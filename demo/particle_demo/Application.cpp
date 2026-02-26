//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Compilation unit for the Application class.
//
//   Implements construction, destruction, settings initialization, global cache setup, graphics window creation, and
//   the main state machine run loop.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "Application.h"
#include "engines/EngineMenu.h"
#include "engines/EngineParticleSimulator.h"

#include <iostream>

//=====================================================================================================================
// Constructors
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// Constructor 1/1: Application
//
// Description:
//
//   Construct the application by loading settings, initializing the global cache with default particle counts, and
//   creating the SDL graphics window and renderer.
//
//---------------------------------------------------------------------------------------------------------------------

Application::Application ()
{
	initializeSettings       ();
	initializeGlobalCache    ();
	initializeGraphicsWindow ();
}

//=====================================================================================================================
// Destructor
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// Destructor: ~Application
//
// Description:
//
//   Shut down the SDL renderer, destroy the window, and terminate SDL.
//
//---------------------------------------------------------------------------------------------------------------------

Application::~Application ()
{
	sdlRenderer.shutdown ();
	window.destroy       ();
	SDL_Quit             ();
}

//=====================================================================================================================
// Methods
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// Method: initializeSettings
//
// Description:
//
//   Load the application settings from the properties file and extract the application name, screen width, and
//   screen height.
//
//---------------------------------------------------------------------------------------------------------------------

void Application::initializeSettings ()
{
	settings.load ( "resources/settings.properties" );

	applicationName = settings.getString ( "Application.Name"          );
	screenWidth     = settings.getInt    ( "Application.Screen.Width"  );
	screenHeight    = settings.getInt    ( "Application.Screen.Height" );
}

//---------------------------------------------------------------------------------------------------------------------
// Method: initializeGlobalCache
//
// Description:
//
//   Populate the global cache with the initial application state and default particle count values from
//   the application settings.
//
//---------------------------------------------------------------------------------------------------------------------

void Application::initializeGlobalCache ()
{
	globalCache.put ( "applicationState",    static_cast< int >( STATE_STARTING )                );
	globalCache.put ( "particleCountRed",    settings.getInt ( "Particle.Count.Red.Default" )    );
	globalCache.put ( "particleCountGreen",  settings.getInt ( "Particle.Count.Green.Default" )  );
	globalCache.put ( "particleCountBlue",   settings.getInt ( "Particle.Count.Blue.Default" )   );
	globalCache.put ( "particleCountYellow", settings.getInt ( "Particle.Count.Yellow.Default" ) );
}

//---------------------------------------------------------------------------------------------------------------------
// Method: initializeGraphicsWindow
//
// Description:
//
//   Create the SDL window with the configured application name and screen dimensions, and initialize
//   the SDL renderer.
//
//---------------------------------------------------------------------------------------------------------------------

void Application::initializeGraphicsWindow ()
{
	// Attempt to create the SDL window. If creation fails, log an error and set the state to idle to prevent the
	// application from entering the run loop.

	if ( !window.create ( applicationName, screenWidth, screenHeight ) )
	{
		std::cerr << "Failed to create window." << std::endl;
		applicationState = STATE_IDLE;
		return;
	}

	sdlRenderer.init ( window.getRenderer () );
}

//---------------------------------------------------------------------------------------------------------------------
// Method: run
//
// Description:
//
//   Execute the main application state machine loop.
//
//   Transitions between menu and simulation engine states until the application state becomes idle, at which point
//   the loop exits.
//
//---------------------------------------------------------------------------------------------------------------------

void Application::run ()
{
	// Set the initial state and enter the main application loop.

	applicationState = STATE_STARTING;

	// Loop until the application reaches the idle state, dispatching to the appropriate engine for each state.

	while ( applicationState != STATE_IDLE )
	{
		// Dispatch based on the current application state. Menu states launch the menu engine, the simulation state
		// launches the particle simulator engine, and stopping/default states transition to idle.

		switch ( applicationState )
		{
			case STATE_STARTING:
			{
				// Transition from the initial starting state directly into the main menu.

				applicationState = STATE_MENU_MAIN;
				break;
			}

			case STATE_MENU_MAIN:
			case STATE_MENU_SETTINGS:
			case STATE_MENU_INSTRUCTIONS:

			case STATE_MENU_ABOUT:
			{
				// Any menu state creates and runs the menu engine, which handles all menu screens internally.

				EngineMenu engineMenu ( settings, globalCache, window, sdlRenderer, keyboard );
				engineMenu.run ();

				// Read next state from global cache.

				applicationState = globalCache.get <int> ( "applicationState" );
				break;
			}

			case STATE_LEVEL_PARTICLE_SIMULATION:
			{
				// Create and run the particle simulator engine for the active simulation session.

				EngineParticleSimulator engineParticleSimulator ( settings, globalCache, window, sdlRenderer, keyboard );
				engineParticleSimulator.run ();

				// Read the next application state from the global cache after the simulation engine exits.

				applicationState = globalCache.get <int> ( "applicationState" );
				break;
			}

			case STATE_STOPPING:
			{
				// Graceful shutdown: transition to idle to exit the main loop.

				applicationState = STATE_IDLE;
				break;
			}

			default:
			{
				// Unrecognized state: fall back to idle to prevent an infinite loop.

				applicationState = STATE_IDLE;
				break;
			}
		}
	}
}
