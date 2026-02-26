//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the GlobalCache class, a type-erased key-value store that uses std::any to hold heterogeneous values
//   indexed by string keys.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <any>
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
	// Class: GlobalCache
	//
	// Description:
	//
	//   A global key-value store that maps string keys to type-erased values using std::any. 
	//
	//   - Provides typed retrieval via a templated get method with std::any_cast, returning a default-constructed 
	//     value if the key is not found.
	//
	//   - Useful for sharing configuration, resources, or transient state across engine subsystems.
	//
	//*****************************************************************************************************************

	class GlobalCache
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::unordered_map <std::string, std::any> data;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: get
		//
		// Description:
		//
		//   Retrieve and cast a stored value to the requested type. 
		//
		//   If the key exists, the value is extracted via std::any_cast; otherwise a default-constructed T is returned.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The string key used to look up the cached value.
		//
		// Returns:
		//
		//   The cached value cast to type T, or a default-constructed T if the key is not found.
		//
		//-------------------------------------------------------------------------------------------------------------

		template <typename T>
		T get ( const std::string& key ) const
		{
			// Search the map for the given key and, if found, extract the value via std::any_cast to the requested type.

			auto it = data.find ( key );
			if ( it != data.end () )
			{
				return std::any_cast< T >( it->second );
			}

			// Key not found in the cache; return a default-constructed instance of the requested type.

			return T {};
		}

		//=============================================================================================================
		// Predicate Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: contains
		//
		// Description:
		//
		//   Test whether the cache holds an entry for the specified key.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The string key to search for in the cache.
		//
		// Returns:
		//
		//   True if the cache contains an entry for the given key, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool contains ( const std::string& key ) const
		{
			return data.find ( key ) != data.end ();
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: put
		//
		// Description:
		//
		//   Insert or update a value in the cache under the specified key. 
		//
		//   The value is moved into the internal map, replacing any existing entry with the same key.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The string key under which the value will be stored.
		//
		//   value (std::any):
		//     The value to store, wrapped in a std::any container for type erasure.
		//
		//-------------------------------------------------------------------------------------------------------------

		void put ( const std::string& key, std::any value )
		{
			data [ key ] = std::move ( value );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: remove
		//
		// Description:
		//
		//   Erase the entry associated with the specified key from the cache.
		//
		//   Has no effect if the key does not exist.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The string key of the entry to remove from the cache.
		//
		//-------------------------------------------------------------------------------------------------------------

		void remove ( const std::string& key )
		{
			data.erase ( key );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: clear
		//
		// Description:
		//
		//   Remove all entries from the cache, resetting it to an empty state.
		//
		//-------------------------------------------------------------------------------------------------------------

		void clear ()
		{
			data.clear ();
		}
	};
}
