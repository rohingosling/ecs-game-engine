//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the EngineParticleSimulator class, a specialized Engine subclass that manages the particle
//   simulation scene including ECS registration, entity creation, keyboard input, and frame rendering.
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
// Class: EngineParticleSimulator
//
// Description:
//
//   A specialized Engine subclass that sets up and runs the particle simulation.
//
//   Registers all component types and physics systems, creates particle group and individual particle entities from
//   application settings, processes keyboard input for particle selection and simulation controls, and presents
//   frames via SDL.
//
//*********************************************************************************************************************

class EngineParticleSimulator : public engine::Engine
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
	ecs::Entity worldEntity      = ecs::NULL_ENTITY;
	ecs::Entity hudEntity        = ecs::NULL_ENTITY;
	ecs::Entity selectedParticle = ecs::NULL_ENTITY;

	std::vector <ecs::Entity> particleEntities;
	std::string               resourcePath;

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
	// Constructor 1/1: EngineParticleSimulator
	//
	// Description:
	//
	//   Construct the particle simulator engine with references to shared application resources.
	//
	//   Stores references to settings, global cache, window, renderer, and keyboard, then initializes the ECS world.
	//
	// Arguments:
	//
	//   settings (engine::ApplicationSettings&):
	//     Reference to the application settings for reading simulation configuration.
	//
	//   globalCache (engine::GlobalCache&):
	//     Reference to the global cache for reading particle counts and writing application state.
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

	EngineParticleSimulator
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
	//   Register all ECS component types and systems, create particle group entities, individual particle entities,
	//   and the HUD entity, then configure system references from application settings.
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
	//   Process keyboard input for simulation controls including;
	//   - Escape (deselect or exit)
	//   - Tab (cycle particle selection)
	//   - Arrow keys (accelerate selected particle)
	//   - P (toggle pause)
	//   - T (toggle trails)
	//   - W (toggle wireframe).
	//
	//-----------------------------------------------------------------------------------------------------------------

	void handleKeyboardCommands ();
};
