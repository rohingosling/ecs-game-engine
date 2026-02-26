//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the World class, the central facade of the ECS framework.
//
//   - World coordinates entity creation and destruction, component registration and manipulation, signature
//     construction, and system registration and per-frame updates.
//
//   - All ECS interactions flow through this unified interface.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ComponentManager.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Signature.h"
#include "System.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
	//*****************************************************************************************************************
	// Class: World
	//
	// Description:
	//
	//   Central facade that ties together the EntityManager, ComponentManager, and registered Systems into a single
	//   coherent interface.
	//
	//   - Provides entity lifecycle management, typed component access, variadic signature construction, and ordered
	//     system updates.
	//
	//   - All game-level ECS operations are performed through the World.
	//
	//*****************************************************************************************************************

	class World
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		EntityManager                                              entityManager;
		ComponentManager                                           componentManager;
		std::unordered_map <std::string, std::shared_ptr <System>> systems;
		std::vector <std::string>                                  systemOrder;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getComponent
		//
		// Description:
		//
		//   Retrieve a mutable reference to an entity's component of the specified type.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity whose component is being accessed.
		//
		// Returns:
		//
		//   A mutable reference to the entity's component of type T.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		T& getComponent ( Entity entity )
		{
			return componentManager.getComponent <T> ( entity );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: hasComponent
		//
		// Description:
		//
		//   Check whether an entity has a component of the specified type.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity to query.
		//
		// Returns:
		//
		//   True if the entity possesses a component of type T, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		bool hasComponent ( Entity entity ) const
		{
			return componentManager.hasComponent <T> ( entity );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getComponentBit
		//
		// Description:
		//
		//   Retrieve the bit index assigned to the specified component type.
		//
		// Returns:
		//
		//   The bit index for the component type T in the signature bitset.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		ComponentBit getComponentBit () const
		{
			return componentManager.getBit <T> ();
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getComponentArray
		//
		// Description:
		//
		//   Retrieve the typed ComponentArray for direct iteration over all components of the specified type.
		//
		// Returns:
		//
		//   A shared pointer to the ComponentArray for type T.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		std::shared_ptr <ComponentArray <T>> getComponentArray () const
		{
			return componentManager.getComponentArray <T> ();
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getEntityCount
		//
		// Description:
		//
		//   Return the number of currently living entities in the world.
		//
		// Returns:
		//
		//   The count of entities currently alive.
		//
		//-------------------------------------------------------------------------------------------------------------

		uint32_t getEntityCount () const
		{
			return entityManager.getLivingCount ();
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: isAlive
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
		//-------------------------------------------------------------------------------------------------------------

		bool   isAlive ( Entity entity ) const;

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/1: World
		//
		// Description:
		//
		//   Default constructor.
		//
		//   Initializes the internal EntityManager and ComponentManager to their default states.
		//
		//-------------------------------------------------------------------------------------------------------------

		World () = default;

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
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
		//-------------------------------------------------------------------------------------------------------------

		Entity createEntity ();

		//-------------------------------------------------------------------------------------------------------------
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
		//-------------------------------------------------------------------------------------------------------------

		void destroyEntity ( Entity entity );

		//-------------------------------------------------------------------------------------------------------------
		// Method: registerComponent
		//
		// Description:
		//
		//   Register a component type with the ComponentManager, assigning it a unique bit index.
		//
		//   Must be called before any entity can use this component type.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		void registerComponent ()
		{
			componentManager.registerComponent <T> ();
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: addComponent
		//
		// Description:
		//
		//   Add a component to an entity, update its signature, and refresh all system entity sets.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The target entity ID.
		//
		//   component (const T&):
		//     The component data to attach to the entity.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		void addComponent ( Entity entity, const T& component )
		{
			componentManager.addComponent <T> ( entity, component );

			// Update entity signature and refresh system entity sets.

			auto signature = entityManager.getSignature ( entity );
			signature.set ( componentManager.getBit <T> () );
			entityManager.setSignature ( entity, signature );

			updateSystemEntitySets ( entity, signature );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: removeComponent
		//
		// Description:
		//
		//   Remove a component from an entity, update its signature, and refresh all system entity sets.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The target entity ID.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		void removeComponent ( Entity entity )
		{
			componentManager.removeComponent <T> ( entity );

			auto signature = entityManager.getSignature ( entity );
			signature.reset ( componentManager.getBit <T> () );
			entityManager.setSignature ( entity, signature );

			updateSystemEntitySets ( entity, signature );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: makeSignature
		//
		// Description:
		//
		//   Construct a Signature bitset with the bits set for all specified component types using a variadic
		//   fold expression.
		//
		//   Example: `world.makeSignature <Physics, Transform> ()`.
		//
		// Returns:
		//
		//   A Signature with the bits set for each specified component type.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename... Components>
		Signature makeSignature () const
		{
			Signature signature;
			( signature.set ( componentManager.getBit <Components> () ), ... );
			return signature;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: registerSystem
		//
		// Description:
		//
		//   Create and register a new system of the specified type with a name and required component signature.
		//
		//   The system is added to the ordered update list and returned as a shared pointer.
		//
		// Arguments:
		//
		//   name (const std::string&):
		//     A unique name identifying the system.
		//
		//   signature (Signature):
		//     The component signature specifying which components an entity must have to be processed.
		//
		// Returns:
		//
		//   A shared pointer to the newly created system instance.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		std::shared_ptr <T> registerSystem ( const std::string& name, Signature signature )
		{
			auto system = std::make_shared <T> ();
			system->name      = name;
			system->signature = signature;

			systems [ name ] = system;
			systemOrder.push_back ( name );

			return system;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: getSystem
		//
		// Description:
		//
		//   Retrieve a registered system by name, cast to the specified derived type.
		//
		// Arguments:
		//
		//   name (const std::string&):
		//     The name of the system to retrieve.
		//
		// Returns:
		//
		//   A shared pointer to the system cast to type T, or nullptr if not found.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		std::shared_ptr <T> getSystem ( const std::string& name ) const
		{
			auto it = systems.find ( name );
			if ( it != systems.end () )
			{
				return std::static_pointer_cast< T >( it->second );
			}
			return nullptr;
		}

		//-------------------------------------------------------------------------------------------------------------
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
		//-------------------------------------------------------------------------------------------------------------

		void updateSystems ( double dt );

	private:

		//-------------------------------------------------------------------------------------------------------------
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
		//-------------------------------------------------------------------------------------------------------------

		void updateSystemEntitySets ( Entity entity, Signature entitySignature );
	};
}
