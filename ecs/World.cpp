//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Implementation of the World class's non-template methods:
//   - entity lifecycle (createEntity, destroyEntity, isAlive)
//   - system update dispatch
//   - system entity set maintenance
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "World.h"

//---------------------------------------------------------------------------------------------------------------------
// Namespace: ecs
//
// Description:
//
//   Core namespace for the Entity Component System framework.
//
//   Contains all ECS types, managers, and system abstractions used to compose game objects through data-driven 
//   entity-component relationships.
//
//---------------------------------------------------------------------------------------------------------------------

namespace ecs
{
	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: createEntity
	//
	// Description:
	//
	//   Allocate a new entity by delegating to the internal EntityManager.
	//
	// Returns:
	//
	//   The newly created entity ID.
	//
	//-----------------------------------------------------------------------------------------------------------------

	Entity World::createEntity ()
	{
		return entityManager.createEntity ();
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: destroyEntity
	//
	// Description:
	//
	//   Destroy an entity, removing it from all system entity sets, cleaning up its components, and recycling
	//   its ID.
	//
	// Arguments:
	//
	//   entity (Entity):
	//     The entity ID to destroy.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void World::destroyEntity ( Entity entity )
	{
		// Remove from all system entity sets.

		for ( auto& [ name, system ] : systems )
		{
			system->entities.erase ( entity );
		}

		// Clean up component arrays and recycle the ID.

		componentManager.entityDestroyed ( entity );
		entityManager.destroyEntity      ( entity );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: isAlive
	//
	// Description:
	//
	//   Check whether an entity is currently alive by verifying it has a non-empty component signature.
	//
	// Arguments:
	//
	//   entity (Entity):
	//     The entity ID to query.
	//
	// Returns:
	//
	//   True if the entity exists and has a non-empty signature, false otherwise.
	//
	//-----------------------------------------------------------------------------------------------------------------

	bool World::isAlive ( Entity entity ) const
	{
		// Reject the null sentinel and any ID that falls outside the valid entity range.

		if ( entity == NULL_ENTITY || entity > MAX_ENTITIES )
		{
			return false;
		}

		// An entity is considered alive if its signature has at least one bit set, meaning it
		// possesses one or more components.
		//
		// A zeroed-out signature indicates the ID has been recycled or was never initialized.

		return entityManager.getSignature ( entity ).any ();
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: updateSystems
	//
	// Description:
	//
	//   Invoke update on all registered systems in registration order, skipping any that are disabled.
	//
	// Arguments:
	//
	//   dt (double):
	//     Delta time in seconds since the previous frame.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void World::updateSystems ( double dt )
	{
		// Iterate over systems in the order they were registered. 
		//
		// - Each enabled system receives the current delta time and a reference to this World so it can query and 
		//   modify entities and components.
		//
		// - Disabled systems are skipped entirely for the frame.

		for ( const auto& name : systemOrder )
		{
			auto& system = systems [ name ];

			if ( system->enabled )
			{
				system->update ( *this, dt );
			}
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: updateSystemEntitySets
	//
	// Description:
	//
	//   Refresh every registered system's entity set after an entity's signature changes. 
	//
	//   Adds or removes the entity based on whether its signature matches each system's required signature.
	//
	// Arguments:
	//
	//   entity (Entity):
	//     The entity whose system membership is being refreshed.
	//
	//   entitySignature (Signature):
	//     The entity's current component signature after the change.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void World::updateSystemEntitySets ( Entity entity, Signature entitySignature )
	{
		// Walk every registered system and test the entity's signature against it.

		for ( auto& [ name, system ] : systems )
		{
			// Bitwise AND the entity's signature with the system's required signature. 
			//
			// - If the result equals the system signature, every required component bit is present and the
			//   entity qualifies for this system.
			//
			// - Otherwise, remove it in case it was previously a member but lost a required component.

			const Signature& systemSig = system->signature;

			if ( ( entitySignature & systemSig ) == systemSig )
			{
				system->entities.insert ( entity );
			}
			else
			{
				system->entities.erase ( entity );
			}
		}
	}
}
