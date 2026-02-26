//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ApplicationSettings class, an INI-style settings loader that parses key-value pairs from a text file
//   and provides typed accessors for string, integer, double, and boolean values.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
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
	// Class: ApplicationSettings
	//
	// Description:
	//
	//   Loads and stores application settings from an INI-style text file as key-value string pairs.
	//
	//   Provides typed accessors for string, integer, double, and boolean values, throwing an exception if 
	//   a requested key is missing.
	//
	//*****************************************************************************************************************

	class ApplicationSettings
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::unordered_map <std::string, std::string> properties;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getString
		//
		// Description:
		//
		//   Retrieve the raw string value associated with the specified key.
		//
		//   Throws std::runtime_error if the key is not found.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The property key to look up.
		//
		// Returns:
		//
		//   The string value associated with the key.
		//
		//-------------------------------------------------------------------------------------------------------------

		std::string getString ( const std::string& key ) const
		{
			// Search for the key in the properties map and throw if it does not exist.

			auto it = properties.find ( key );
			if ( it == properties.end () )
			{
				throw std::runtime_error ( "Setting not found: " + key );
			}
			return it->second;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getInt
		//
		// Description:
		//
		//   Retrieve the integer value associated with the specified key by parsing the stored string with std::stoi.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The property key to look up.
		//
		// Returns:
		//
		//   The integer value parsed from the stored string.
		//
		//-------------------------------------------------------------------------------------------------------------

		int getInt ( const std::string& key ) const
		{
			return std::stoi ( getString ( key ) );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getDouble
		//
		// Description:
		//
		//   Retrieve the double value associated with the specified key by parsing the stored string with std::stod.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The property key to look up.
		//
		// Returns:
		//
		//   The double value parsed from the stored string.
		//
		//-------------------------------------------------------------------------------------------------------------

		double getDouble ( const std::string& key ) const
		{
			return std::stod ( getString ( key ) );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getBool
		//
		// Description:
		//
		//   Retrieve the boolean value associated with the specified key.
		//
		//   Returns true if the stored string is "true", "1", or "yes"; false otherwise.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The property key to look up.
		//
		// Returns:
		//
		//   True if the value is "true", "1", or "yes"; false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool getBool ( const std::string& key ) const
		{
			// Retrieve the string value for the key and return true if it matches any of the recognized truthy literals.

			std::string val = getString ( key );
			return ( val == "true" || val == "1" || val == "yes" );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: hasKey
		//
		// Description:
		//
		//   Check whether a property with the specified key exists in the loaded settings.
		//
		// Arguments:
		//
		//   key (const std::string&):
		//     The property key to search for.
		//
		// Returns:
		//
		//   True if the key exists in the loaded properties, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool hasKey ( const std::string& key ) const
		{
			return properties.find ( key ) != properties.end ();
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getAll
		//
		// Description:
		//
		//   Return a const reference to the entire properties map for direct iteration or inspection.
		//
		// Returns:
		//
		//   A const reference to the unordered map of all loaded key-value properties.
		//
		//-------------------------------------------------------------------------------------------------------------

		const std::unordered_map <std::string, std::string>& getAll () const
		{
			return properties;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/2: ApplicationSettings
		//
		// Description:
		//
		//   Default constructor.
		//
		//   Creates an empty settings container with no loaded properties.
		//
		//-------------------------------------------------------------------------------------------------------------

		ApplicationSettings () = default;

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 2/2: ApplicationSettings
		//
		// Description:
		//
		//   Construct an ApplicationSettings instance and immediately load properties from the specified file path.
		//
		// Arguments:
		//
		//   filePath (const std::string&):
		//     The path to the INI-style settings file to load.
		//
		//-------------------------------------------------------------------------------------------------------------

		explicit ApplicationSettings ( const std::string& filePath )
		{
			load ( filePath );
		}

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: load
		//
		// Description:
		//
		//   Parse an INI-style settings file, extracting key-value pairs separated by '=' delimiters.
		//
		//   Lines starting with '#' or '!' are treated as comments and skipped.
		//
		// Arguments:
		//
		//   filePath (const std::string&):
		//     The path to the settings file to parse.
		//
		//-------------------------------------------------------------------------------------------------------------

		void load ( const std::string& filePath )
		{
			// Open the settings file and throw if it cannot be read.

			std::ifstream file ( filePath );

			if ( !file.is_open () )
			{
				throw std::runtime_error ( "Cannot open settings file: " + filePath );
			}

			// Read the file line by line, skipping comments and blank lines, and extract key-value pairs split on '='.

			std::string line;

			while ( std::getline ( file, line ) )
			{
				// Skip empty lines and comments.

				if ( line.empty () || line [ 0 ] == '#' || line [ 0 ] == '!' )
				{
					continue;
				}

				// Find the '=' delimiter.

				auto pos = line.find ( '=' );
				if ( pos == std::string::npos )
				{
					continue;
				}

				// Extract and trim the key and value substrings on either side of the delimiter, then store the pair.

				std::string key = trim ( line.substr ( 0, pos ) );
				std::string value = trim ( line.substr ( pos + 1 ) );

				if ( !key.empty () )
				{
					properties [ key ] = value;
				}
			}
		}

	private:

		//-------------------------------------------------------------------------------------------------------------
		// Method: trim
		//
		// Description:
		//
		//   Remove leading and trailing whitespace characters (spaces, tabs, carriage returns, newlines) from a
		//   string.
		//
		// Arguments:
		//
		//   s (const std::string&):
		//     The input string to trim.
		//
		// Returns:
		//
		//   A copy of the input string with leading and trailing whitespace removed.
		//
		//-------------------------------------------------------------------------------------------------------------

		static std::string trim ( const std::string& s )
		{
			// Locate the first and last non-whitespace characters. Return an empty string if the input is all whitespace.

			auto start = s.find_first_not_of ( " \t\r\n" );
			if ( start == std::string::npos ) return "";
			auto end = s.find_last_not_of ( " \t\r\n" );

			// Return the substring spanning from the first to the last non-whitespace character, inclusive.

			return s.substr ( start, end - start + 1 );
		}
	};
}
