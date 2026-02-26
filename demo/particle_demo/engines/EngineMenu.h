//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the EngineMenu class, a specialized Engine subclass that manages the menu UI including
//   component registration, entity creation, keyboard input routing, and frame rendering.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/Engine.h"
#include "../../../engine/ApplicationSettings.h"
#include "../../../engine/GlobalCache.h"
#include "../../../engine/platform/SDLWindow.h"
#include "../../../engine/platform/SDLRenderer.h"
#include "../../../engine/platform/SDLKeyboard.h"

//*********************************************************************************************************************
// Class: EngineMenu
//
// Description:
//
//   A specialized Engine subclass that sets up and runs the menu system.
//
//   Registers menu component types and systems, creates button, background, and text box entities from application
//   settings, routes keyboard input to the SystemMenuManager, and presents frames via SDL.
//
//*********************************************************************************************************************

class EngineMenu : public engine::Engine
{
private:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::ApplicationSettings& settings;
	engine::GlobalCache&         globalCache;
	engine::SDLWindow&           window;
	engine::SDLRenderer&         sdlRenderer;
	engine::SDLKeyboard&         keyboard;
	std::string                  resourcePath;

	//=================================================================================================================
	// Accessors
	//=================================================================================================================

	// None

	//=================================================================================================================
	// Mutators
	//=================================================================================================================

	// None

public:

	//=================================================================================================================
	// Constructors
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Constructor 1/1: EngineMenu
	//
	// Description:
	//
	//   Construct the menu engine with references to shared application resources.
	//
	//   Stores references to settings, global cache, window, renderer, and keyboard, then initializes the ECS world.
	//
	// Arguments:
	//
	//   settings (engine::ApplicationSettings&):
	//     Reference to the application settings for reading menu configuration.
	//
	//   globalCache (engine::GlobalCache&):
	//     Reference to the global cache for reading and writing application state.
	//
	//   window (engine::SDLWindow&):
	//     Reference to the SDL window for frame presentation.
	//
	//   sdlRenderer (engine::SDLRenderer&):
	//     Reference to the SDL renderer for drawing operations.
	//
	//   keyboard (engine::SDLKeyboard&):
	//     Reference to the SDL keyboard handler for polling input events.
	//
	//-----------------------------------------------------------------------------------------------------------------

	EngineMenu
	(
		engine::ApplicationSettings& settings,
		engine::GlobalCache&         globalCache,
		engine::SDLWindow&           window,
		engine::SDLRenderer&         sdlRenderer,
		engine::SDLKeyboard&         keyboard
	);

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: swapBuffer
	//
	// Description:
	//
	//   Present the current frame to the window and process keyboard input.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void swapBuffer () override;

private:

	//-----------------------------------------------------------------------------------------------------------------
	// Method: initialize
	//
	// Description:
	//
	//   Register all menu component types and systems, create button and text box entities from application
	//   settings, and configure system references.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void initialize ();

	//-----------------------------------------------------------------------------------------------------------------
	// Method: processInput
	//
	// Description:
	//
	//   Poll SDL events and dispatch keyboard commands.
	//
	//   Posts a deferred stop command if the window is closed.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void processInput ();

	//-----------------------------------------------------------------------------------------------------------------
	// Method: handleKeyboardCommands
	//
	// Description:
	//
	//   Map keyboard key presses to SystemMenuManager action codes for navigation, selection, and
	//   increment/decrement operations.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void handleKeyboardCommands ();
};
