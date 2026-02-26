//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the EntityManager class, which handles entity lifecycle management for the ECS framework.
//
//   Entities are allocated from a recycling ID pool and tracked via per-entity component signatures stored in a
//   fixed-size array.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "Entity.h"
#include "Signature.h"

#include <array>
#include <cassert>
#include <queue>

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
	// Class: EntityManager
	//
	// Description:
	//
	//   Manages the full lifecycle of entities within the ECS framework.
	//
	//   - Maintains a recycling pool of entity IDs using a FIFO queue, a fixed-size array of component signatures
	//     indexed by entity ID, and a count of currently living entities.
	//
	//   - Entity ID 0 is reserved as NULL_ENTITY and is never allocated.
	//
	//*****************************************************************************************************************

	class EntityManager
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::queue <Entity>                           availableIds;
		std::array <Signature, MAX_ENTITIES + 1>      signatures {};
		uint32_t                                      livingCount = 0;

	public:

		//=============================================================================================================
		// Value Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getSignature
		//
		// Description:
		//
		//   Retrieve the component signature for the specified entity.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The target entity ID. Must be in the valid range (1 .. MAX_ENTITIES).
		//
		// Returns:
		//
		//   A copy of the entity's current component signature bitset.
		//
		//-------------------------------------------------------------------------------------------------------------

		Signature getSignature ( Entity entity ) const
		{
			assert ( entity > NULL_ENTITY && entity <= MAX_ENTITIES && "Entity out of range." );
			return signatures [ entity ];
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getLivingCount
		//
		// Description:
		//
		//   Return the number of entities currently alive in the manager.
		//
		// Returns:
		//
		//   The count of currently living entities.
		//
		//-------------------------------------------------------------------------------------------------------------

		uint32_t getLivingCount () const
		{
			return livingCount;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Mutator: setSignature
		//
		// Description:
		//
		//   Assign a component signature to the specified entity, replacing any existing signature.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The target entity ID. Must be in the valid range (1 .. MAX_ENTITIES).
		//
		//   signature (Signature):
		//     The new component signature bitset to assign.
		//
		//-------------------------------------------------------------------------------------------------------------

		void setSignature ( Entity entity, Signature signature )
		{
			assert ( entity > NULL_ENTITY && entity <= MAX_ENTITIES && "Entity out of range." );
			signatures [ entity ] = signature;
		}

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/1: EntityManager
		//
		// Description:
		//
		//   Initialize the entity manager by pre-filling the available ID queue with all valid entity IDs from 1
		//   through MAX_ENTITIES.
		//
		//   Entity ID 0 is reserved as NULL_ENTITY and excluded from the pool.
		//
		//-------------------------------------------------------------------------------------------------------------

		EntityManager ()
		{
			// Pre-fill the queue with all valid entity IDs (1 .. MAX_ENTITIES).
			// ID 0 is reserved as NULL_ENTITY.

			for ( Entity e = 1; e <= MAX_ENTITIES; ++e )
			{
				availableIds.push ( e );
			}
		}

		//=============================================================================================================
		// Destructors
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: createEntity
		//
		// Description:
		//
		//   Allocate and return the next available entity ID from the recycling pool.
		//
		//   Asserts if the maximum entity count has been reached.
		//
		// Returns:
		//
		//   The newly allocated entity ID.
		//
		//-------------------------------------------------------------------------------------------------------------

		Entity createEntity ()
		{
			assert ( livingCount < MAX_ENTITIES && "Too many entities." );

			Entity id = availableIds.front ();
			availableIds.pop ();
			++livingCount;

			return id;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: destroyEntity
		//
		// Description:
		//
		//   Destroy an entity by resetting its component signature, returning its ID to the available pool, and
		//   decrementing the living entity count.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID to destroy. Must be in the valid range (1 .. MAX_ENTITIES).
		//
		//-------------------------------------------------------------------------------------------------------------

		void destroyEntity ( Entity entity )
		{
			assert ( entity > NULL_ENTITY && entity <= MAX_ENTITIES && "Entity out of range." );

			signatures [ entity ].reset ();
			availableIds.push ( entity );
			--livingCount;
		}
	};
}
