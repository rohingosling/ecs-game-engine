//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Hello World
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Entry point for the Hello World demo application.
//
//   Registers components and systems, creates a message entity, spawns a background input thread to wait for any key
//   press, and runs the engine's main loop.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "../../ecs/World.h"
#include "../../engine/Engine.h"
#include "components/TextComponent.h"
#include "components/MessageStatusComponent.h"
#include "systems/TerminalSystem.h"

#include <conio.h>
#include <iostream>
#include <thread>

//---------------------------------------------------------------------------------------------------------------------
// Method: main
//
// Description:
//
//   Application entry point.
//
//   - Initializes the engine, registers ECS components and the TerminalSystem, creates a "Hello World!" message
//     entity, and launches the main game loop.
//
//   - A detached background thread waits for any key press and posts a deferred stop command to exit gracefully.
//
// Returns:
//
//   Exit code 0 on successful termination.
//
//---------------------------------------------------------------------------------------------------------------------

int main ()
{
	// Initialize the engine and configure the ECS world.

	engine::Engine engine;

	auto& world = engine.getWorld ();

	// Register components.

	world.registerComponent<TextComponent> ();
	world.registerComponent<MessageStatusComponent> ();

	// Register the terminal system.

	auto signature = world.makeSignature<TextComponent, MessageStatusComponent> ();
	world.registerSystem<TerminalSystem> ( "TerminalSystem", signature );

	// Create the message entity.

	auto message = world.createEntity ();
	world.addComponent ( message, TextComponent { "Hello World!" } );
	world.addComponent ( message, MessageStatusComponent {} );

	// Background thread waits for any key press, then posts an exit command.

	std::thread inputThread
	(
		[ &engine ] ()
		{
			_getch ();
			engine.getCommandManager ().post ( [ &engine ] (){ engine.stop (); } );
		}
	);	
	inputThread.detach ();

	// Run the game loop (default 90 FPS, exits when stop() is called).

	engine.run ();

	// Engine has stopped; exit successfully.

	return 0;
}
