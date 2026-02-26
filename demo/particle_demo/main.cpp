//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Entry point for the Particle Simulator demo application.
//
//   Sets the working directory to the executable's location and creates and runs the Application instance.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "Application.h"

#include <filesystem>
#include <iostream>

//---------------------------------------------------------------------------------------------------------------------
// Method: main
//
// Description:
//
//   Application entry point.
//
//   Sets the working directory to the executable's parent directory so that relative resource paths resolve
//   correctly, then creates and runs the Application.
//
// Arguments:
//
//   argc (int):
//     The number of command-line arguments.
//
//   argv[] (char*):
//     Array of command-line argument strings.
//
// Returns:
//
//   Exit code 0 on successful termination, or 1 if a fatal exception occurs.
//
//---------------------------------------------------------------------------------------------------------------------

int main ( int argc, char* argv [] )
{
	try
	{
		// Set working directory to the executable's location so that relative resource paths work regardless of how
		// the app is launched.

		std::filesystem::path exePath ( argv [ 0 ] );
		std::filesystem::path exeDir = std::filesystem::weakly_canonical ( exePath ).parent_path ();

		if ( !exeDir.empty () )
		{
			std::filesystem::current_path ( exeDir );
		}

		// Create the application instance and run the main state machine loop until the user exits.

		Application app;
		app.run ();
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Fatal error: " << e.what () << std::endl;
		return 1;
	}

	// Application completed successfully.

	return 0;
}
