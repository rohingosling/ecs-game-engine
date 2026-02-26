//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the abstract Resource base class and the ResourceManager, which stores and manages named resources via
//   shared pointers.
//
//   Resources support virtual load and unload lifecycle methods.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

//---------------------------------------------------------------------------------------------------------------------
// Namespace: engine
//
// Description:
//
//   Core namespace for the game engine framework. 
//
//   Contains math utilities, platform abstractions, resource management, and application infrastructure used to build
//   game applications on top of the ECS layer.
//
//---------------------------------------------------------------------------------------------------------------------

namespace engine
{
	//*****************************************************************************************************************
	// Class: Resource
	//
	// Description:
	//
	//   Abstract base class for all managed resources.
	//
	//   - Provides virtual load and unload lifecycle methods that derived classes override to handle
	//     resource-specific initialization and cleanup.
	//
	//   - Stores the file name associated with the resource.
	//
	//*****************************************************************************************************************

	class Resource
	{
	public:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::string fileName;

		//=============================================================================================================
		// Destructor
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Destructor: ~Resource
		//
		// Description:
		//
		//   Virtual destructor ensuring proper cleanup of derived resource types when destroyed through a base pointer.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual ~Resource () = default;

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: load
		//
		// Description:
		//
		//   Virtual hook invoked to load the resource from its associated file. 
		//
		//   Default implementation is a no-op.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual void load () {}

		//-------------------------------------------------------------------------------------------------------------
		// Method: unload
		//
		// Description:
		//
		//   Virtual hook invoked to release the resource and free associated memory. 
		//
		//   Default implementation is a no-op.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual void unload () {}
	};

	//*****************************************************************************************************************
	// Class: ResourceManager
	//
	// Description:
	//
	//   Manages a collection of named resources stored as shared pointers. 
	//
	//   Provides methods to add, load, unload, and  retrieve resources by key, as well as batch load and unload 
	//   operations for all registered resources.
	//
	//*****************************************************************************************************************

	class ResourceManager
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::unordered_map <std::string, std::shared_ptr <Resource>> resources;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: get
		//
		// Description:
		//
		//   Retrieve a resource by key, cast to the specified derived type via std::static_pointer_cast. 
		//
		//   Returns a nullptr if the key is not found.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The key identifying the resource to retrieve.
		//
		// Returns:
		//
		//   A shared pointer to the resource cast to type T, or nullptr if the key is not found.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		std::shared_ptr <T> get ( const std::string& key ) const
		{
			// Look up the resource by key and, if found, downcast the base shared pointer to the requested derived type.

			auto it = resources.find ( key );
			if ( it != resources.end () )
			{
				return std::static_pointer_cast< T >( it->second );
			}

			// Key not found in the resource map; return nullptr to signal that no resource is registered under this key.

			return nullptr;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: add
		//
		// Description:
		//
		//   Register a resource under the specified key. 
		//
		//   The resource is moved into the internal map, replacing any existing entry with the same key.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The unique string identifier for the resource.
		//
		//   resource (std::shared_ptr <Resource>):
		//     A shared pointer to the resource to register.
		//
		//-------------------------------------------------------------------------------------------------------------

		void add ( const std::string& key, std::shared_ptr <Resource> resource )
		{
			resources [ key ] = std::move ( resource );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: load
		//
		// Description:
		//
		//   Load a single resource identified by key by invoking its virtual load method. 
		//
		//   Has no effect if the key is not found.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The key identifying the resource to load.
		//
		//-------------------------------------------------------------------------------------------------------------

		void load ( const std::string& key )
		{
			// Search for the resource by key and invoke its virtual load method if the entry exists.

			auto it = resources.find ( key );
			if ( it != resources.end () )
			{
				it->second->load ();
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: loadAll
		//
		// Description:
		//
		//   Invoke the load method on every registered resource in the manager.
		//
		//-------------------------------------------------------------------------------------------------------------

		void loadAll ()
		{
			for ( auto& [ key, resource ] : resources )
			{
				resource->load ();
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: unload
		//
		// Description:
		//
		//   Unload a single resource identified by key by invoking its virtual unload method. 
		//
		//   Has no effect if the key is not found.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The key identifying the resource to unload.
		//
		//-------------------------------------------------------------------------------------------------------------

		void unload ( const std::string& key )
		{
			// Search for the resource by key and invoke its virtual unload method if the entry exists.

			auto it = resources.find ( key );
			if ( it != resources.end () )
			{
				it->second->unload ();
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: unloadAll
		//
		// Description:
		//
		//   Invoke the unload method on every registered resource in the manager.
		//
		//-------------------------------------------------------------------------------------------------------------

		void unloadAll ()
		{
			// Iterate over every entry in the resource map and invoke unload on each to release all managed resources.

			for ( auto& [key, resource] : resources )
			{
				resource->unload ();
			}
		}
	};
}
