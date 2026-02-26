//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Application class, the top-level application controller that manages initialization,
//   state machine transitions, and the main run loop for the Particle Simulator demo.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../engine/ApplicationSettings.h"
#include "../../engine/GlobalCache.h"
#include "../../engine/platform/SDLWindow.h"
#include "../../engine/platform/SDLRenderer.h"
#include "../../engine/platform/SDLKeyboard.h"

#include <string>

//*********************************************************************************************************************
// Class: Application
//
// Description:
//
//   Top-level application controller that owns SDL platform resources (window, renderer, keyboard), application
//   settings, and a global cache.
//
//   Manages a state machine that transitions between menu screens and the particle simulation engine, creating and
//   running sub-engines as needed.
//
//*********************************************************************************************************************

class Application
{
public:

	//=================================================================================================================
	// Constants
	//=================================================================================================================

	static constexpr int STATE_IDLE                      = 0;
	static constexpr int STATE_STARTING                  = 1;
	static constexpr int STATE_STOPPING                  = 2;
	static constexpr int STATE_MENU_MAIN                 = 10;
	static constexpr int STATE_MENU_SETTINGS             = 11;
	static constexpr int STATE_MENU_INSTRUCTIONS         = 12;
	static constexpr int STATE_MENU_ABOUT                = 13;
	static constexpr int STATE_LEVEL_PARTICLE_SIMULATION = 100;

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	// None

	//=================================================================================================================
	// Accessors
	//=================================================================================================================

	// None

	//=================================================================================================================
	// Mutators
	//=================================================================================================================

	// None

	//=================================================================================================================
	// Constructors
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Constructor 1/1: Application
	//
	// Description:
	//
	//   Construct the application by loading settings, initializing the global cache with default particle counts,
	//   and creating the SDL graphics window and renderer.
	//
	//-----------------------------------------------------------------------------------------------------------------

	Application ();

	//=================================================================================================================
	// Destructor
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Destructor: ~Application
	//
	// Description:
	//
	//   Shut down the SDL renderer, destroy the window, and terminate SDL.
	//
	//-----------------------------------------------------------------------------------------------------------------

	~Application ();

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: run
	//
	// Description:
	//
	//   Execute the main application state machine loop.
	//
	//   Transitions between menu and simulation engine states until the application state becomes idle, at which point
	//   the loop exits.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void run ();

private:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::ApplicationSettings settings;
	engine::GlobalCache         globalCache;
	engine::SDLWindow           window;
	engine::SDLRenderer         sdlRenderer;
	engine::SDLKeyboard         keyboard;

	std::string applicationName;
	int         screenWidth      = 1920;
	int         screenHeight     = 1080;
	int         applicationState = STATE_STARTING;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: initializeSettings
	//
	// Description:
	//
	//   Load the application settings from the properties file and extract the application name, screen  width, and
	//   screen height.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void initializeSettings ();

	//-----------------------------------------------------------------------------------------------------------------
	// Method: initializeGlobalCache
	//
	// Description:
	//
	//   Populate the global cache with the initial application state and default particle count values from the
	//   application settings.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void initializeGlobalCache ();

	//-----------------------------------------------------------------------------------------------------------------
	// Method: initializeGraphicsWindow
	//
	// Description:
	//
	//   Create the SDL window with the configured application name and screen dimensions, and initialize
	//   the SDL renderer.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void initializeGraphicsWindow ();
};
