//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentManager class, which coordinates component type registration and per-entity component
//   storage for the ECS framework.
//
//   Maps each component type to a unique bit index and a type-erased ComponentArray.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ComponentArray.h"
#include "Entity.h"
#include "Signature.h"

#include <cassert>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

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
	// Class: ComponentManager
	//
	// Description:
	//
	//   Manages registration and storage of all component types in the ECS framework. 
	//
	//   - Maintains a mapping from each component's std::type_index to a unique bit position for use in entity
	//     signatures, and to a type-erased ComponentArray that holds the actual component data.
	//
	//   - Provides typed accessors for adding, removing, and querying components on a per-entity basis.
	//
	//*****************************************************************************************************************

	class ComponentManager
	{	
		//=============================================================================================================
		// Data Members
		//=============================================================================================================

	private:		

		std::unordered_map <std::type_index, ComponentBit>                       typeToBit;
		std::unordered_map <std::type_index, std::shared_ptr <IComponentArray>>  typeToArray;
		ComponentBit                                                             nextBit = 0;

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

	public:

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getBit
		//
		// Description:
		//
		//   Retrieve the bit index assigned to the specified component type.
		//
		//   Asserts if the component type has not been registered.
		//
		// Returns:
		//
		//   The bit index for the component type T in the signature bitset.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		ComponentBit getBit () const
		{
			// Obtain the runtime type index for template type T to use as a lookup key in the bit map.

			std::type_index ti ( typeid( T ) );

			// Look up the type in the bit map and assert that it was previously registered.

			auto it = typeToBit.find ( ti );
			assert ( it != typeToBit.end () && "Component not registered before use." );

			// Return the bit index assigned to this component type.

			return it->second;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getComponent
		//
		// Description:
		//
		//   Retrieve a mutable reference to the component of type T for the specified entity.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID whose component is being accessed.
		//
		// Returns:
		//
		//   A mutable reference to the entity's component of type T.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		T& getComponent ( Entity entity )
		{
			// Delegate to the typed ComponentArray to retrieve the component for the given entity.

			return getComponentArray <T> ()->get ( entity );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: hasComponent
		//
		// Description:
		//
		//   Check whether the specified entity has a component of type T.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID to query for component presence.
		//
		// Returns:
		//
		//   True if the entity possesses a component of type T, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		bool hasComponent ( Entity entity ) const
		{
			// Delegate to the typed ComponentArray to check whether the entity has a component of this type.

			return getComponentArray <T> ()->has ( entity );
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: registerComponent
		//
		// Description:
		//
		//   Register a new component type by assigning it the next available bit index and creating its backing
		//   ComponentArray.
		//
		//   Asserts if the component type has already been registered.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		void registerComponent ()
		{
			// Obtain the runtime type index for template type T to use as the registration key.

			std::type_index ti ( typeid( T ) );

			// Assert that this component type has not already been registered to prevent duplicate bit assignments.

			assert ( typeToBit.find ( ti ) == typeToBit.end () && "Registering component type more than once." );

			// Map the type to the current bit index, create a new typed ComponentArray for storage, and advance the
			// bit counter.

			typeToBit   [ ti ] = nextBit;
			typeToArray [ ti ] = std::make_shared <ComponentArray <T>> ();
			nextBit++;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: addComponent
		//
		// Description:
		//
		//   Add a component of type T to the specified entity by delegating to the appropriate typed ComponentArray.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The target entity ID to attach the component to.
		//
		//   component (const T&):
		//     The component data to copy into the entity's component storage.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		void addComponent ( Entity entity, const T& component )
		{
			// Delegate to the typed ComponentArray to insert the component data for the given entity.

			getComponentArray <T> ()->insert ( entity, component );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: removeComponent
		//
		// Description:
		//
		//   Remove the component of type T from the specified entity by delegating to the appropriate typed
		//   ComponentArray.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID whose component of type T should be removed.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		void removeComponent ( Entity entity )
		{
			// Delegate to the typed ComponentArray to remove the component data for the given entity.

			getComponentArray <T> ()->remove ( entity );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: entityDestroyed
		//
		// Description:
		//
		//   Notify all registered ComponentArrays that an entity has been destroyed, allowing each to remove the
		//   entity's component data if present.
		//
		// Arguments:
		//
		//   entity (Entity):
		//     The entity ID that was destroyed and whose component data should be cleaned up.
		//
		//-------------------------------------------------------------------------------------------------------------

		void entityDestroyed ( Entity entity )
		{
			// Iterate over every registered ComponentArray and notify each one to clean up the destroyed entity's data.

			for ( auto& [type, array] : typeToArray )
			{
				array->entityDestroyed ( entity );
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: getComponentArray
		//
		// Description:
		//
		//   Retrieve the typed ComponentArray for the specified component type.
		//
		//   Asserts if the component type has not been registered.
		//
		// Returns:
		//
		//   A shared pointer to the ComponentArray for type T.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		std::shared_ptr <ComponentArray <T>> getComponentArray () const
		{
			// Obtain the runtime type index for template type T to use as a lookup key in the array map.

			std::type_index ti ( typeid( T ) );

			// Look up the type in the array map and assert that it was previously registered.

			auto it = typeToArray.find ( ti );
			assert ( it != typeToArray.end () && "Component not registered before use." );

			// Downcast the type-erased shared pointer to the concrete ComponentArray type and return it.

			return std::static_pointer_cast< ComponentArray <T> >( it->second );
		}
	};
}
