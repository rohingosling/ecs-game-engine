//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the IComponentArray interface and the templated ComponentArray class for the ECS framework.
//
//   ComponentArray stores components of a single type in a dense, contiguous vector with O(1) lookup via
//   entity-to-index mapping.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "Entity.h"

#include <cassert>
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
	// Class: IComponentArray
	//
	// Description:
	//
	//   Abstract interface for type-erased component arrays. 
	//
	//   Provides a virtual entityDestroyed method so that the ComponentManager can notify all component arrays when 
	//   an entity is destroyed without knowing their concrete types.
	//
	//*****************************************************************************************************************

	class IComponentArray
	{
	public:

		//=============================================================================================================
		// Destructor
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Destructor: ~IComponentArray
		//
		// Description:
		//
		//   Virtual destructor ensuring proper cleanup of derived ComponentArray instances when destroyed through a
		//   base pointer.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual ~IComponentArray () = default;

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: entityDestroyed
		//
		// Description:
		//
		//   Notify this component array that an entity has been destroyed so it can remove the entity's component
		//   data if present.
		//
		//   Pure virtual; implemented by each typed ComponentArray specialization.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID that was destroyed and whose component data should be cleaned up.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual void entityDestroyed ( Entity entity ) = 0;
	};

	//*****************************************************************************************************************
	// Class: ComponentArray
	//
	// Description:
	//
	//   Typed, dense component storage for a single component type T.
	//
	//   - Maintains a contiguous vector of component data alongside bidirectional entity-to-index and
	//     index-to-entity mappings.
	//
	//   - Removals use a swap-with-last strategy to keep the data array tightly packed for cache-friendly iteration.
	//
	//*****************************************************************************************************************

	template <typename T>
	class ComponentArray : public IComponentArray
	{
		//=============================================================================================================
		// Data Members
		//=============================================================================================================

	private:

		std::vector <T>                          components;
		std::unordered_map <Entity, std::size_t> entityToIndex;
		std::vector <Entity>                     indexToEntity;

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

	public:

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: get
		//
		// Description:
		//
		//   Retrieve a mutable reference to the component associated with the given entity.
		//
		//   Asserts if the entity has no component of this type.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID whose component is being accessed. Must exist in this array.
		//
		// Returns:
		//
		//   A mutable reference to the entity's component of type T.
		//
		//-------------------------------------------------------------------------------------------------------------

		T& get ( Entity entity )
		{
			// Assert that the entity exists in the map, then return a reference to its component via the index lookup.

			assert ( entityToIndex.find ( entity ) != entityToIndex.end () && "Retrieving non-existent component." );
			return components [ entityToIndex [ entity ] ];
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getData
		//
		// Description:
		//
		//   Return a mutable reference to the underlying dense component vector for direct iteration.
		//
		// Returns:
		//
		//   A mutable reference to the internal vector of components.
		//
		//-------------------------------------------------------------------------------------------------------------

		std::vector <T>& getData ()
		{
			// Expose the dense component vector directly so callers can iterate over all active components.

			return components;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getData
		//
		// Description:
		//
		//   Return a const reference to the underlying dense component vector for read-only iteration.
		//
		// Returns:
		//
		//   A const reference to the internal vector of components.
		//
		//-------------------------------------------------------------------------------------------------------------

		const std::vector <T>& getData () const
		{
			// Return a const reference to the dense component vector for read-only traversal without copying.

			return components;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getEntity
		//
		// Description:
		//
		//   Return the entity ID stored at the given index in the dense array.
		//
		// Arguments:
		//
		//   index (std::size_t):
		//     The zero-based index into the dense component array.
		//
		// Returns:
		//
		//   The entity ID that owns the component at the specified index.
		//
		//-------------------------------------------------------------------------------------------------------------

		Entity getEntity ( std::size_t index ) const
		{
			// Look up the entity that owns the component at the given dense-array index.

			return indexToEntity [ index ];
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: has
		//
		// Description:
		//
		//   Check whether the given entity has a component stored in this array.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID to query for component presence.
		//
		// Returns:
		//
		//   True if the entity has a component in this array, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool has ( Entity entity ) const
		{
			// Return true if the entity-to-index map contains an entry for this entity, indicating a stored component.

			return entityToIndex.find ( entity ) != entityToIndex.end ();
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: size
		//
		// Description:
		//
		//   Return the number of components currently stored in this array.
		//
		// Returns:
		//
		//   The number of active components in the dense array.
		//
		//-------------------------------------------------------------------------------------------------------------

		std::size_t size () const
		{
			// Delegate to the dense vector's size, which reflects the count of live components.

			return components.size ();
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None
		
		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: insert
		//
		// Description:
		//
		//   Insert a new component for the given entity at the end of the dense array.
		//
		//   Asserts if the entity already has a component of this type.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID to associate with the new component. Must not already exist in this array.
		//
		//   component (const T&):
		//     The component data to copy into the dense storage array.
		//
		//-------------------------------------------------------------------------------------------------------------

		void insert ( Entity entity, const T& component )
		{
			// Guard against duplicate insertions; each entity may hold at most one component of any given type.

			assert ( entityToIndex.find ( entity ) == entityToIndex.end () && "Component added to same entity twice." );

			// Record the entity-to-index mapping. The new component will occupy the position at the current end of the
			// dense array.

			std::size_t newIndex     = components.size ();
			entityToIndex [ entity ] = newIndex;

			// Append the entity and its component data to the parallel dense arrays so they remain in sync.

			indexToEntity.push_back ( entity );
			components.push_back    ( component );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: remove
		//
		// Description:
		//
		//   Remove the component associated with the given entity by swapping it with the last element in the dense
		//   array, then popping the back.
		//
		//   - Updates all index mappings accordingly.
		//
		//   - Asserts if the entity has no component.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID whose component should be removed. Must exist in this array.
		//
		//-------------------------------------------------------------------------------------------------------------

		void remove ( Entity entity )
		{
			assert ( entityToIndex.find ( entity ) != entityToIndex.end () && "Removing non-existent component." );

			// Swap the removed element with the last element to keep the array dense.

			std::size_t removedIndex = entityToIndex [ entity ];
			std::size_t lastIndex    = components.size () - 1;

			// If the element being removed is not already the last, move the last element into the vacated slot and
			// update both mapping tables so they reflect the relocated element's new position.

			if ( removedIndex != lastIndex )
			{
				components [ removedIndex ]    = std::move ( components [ lastIndex ] );
				Entity lastEntity              = indexToEntity [ lastIndex ];
				entityToIndex [ lastEntity ]   = removedIndex;
				indexToEntity [ removedIndex ] = lastEntity;
			}

			// Pop the now-redundant tail entries from both dense arrays and erase the removed entity from the map.

			components.pop_back    ();
			indexToEntity.pop_back ();
			entityToIndex.erase    ( entity );
		}

		
		//-------------------------------------------------------------------------------------------------------------
		// Method: entityDestroyed
		//
		// Description:
		//
		//   Handle entity destruction by removing the entity's component from this array if it exists. 
		//
		//   Delegates to remove to maintain dense packing.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID that was destroyed and whose component data should be cleaned up if present.
		//
		//-------------------------------------------------------------------------------------------------------------

		void entityDestroyed ( Entity entity ) override
		{
			// Only attempt removal if this array actually holds a component for the destroyed entity.

			if ( entityToIndex.find ( entity ) != entityToIndex.end () )
			{
				remove ( entity );
			}
		}
	};
}
