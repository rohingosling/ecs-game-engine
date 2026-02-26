//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SDLKeyboard class, which wraps SDL2 keyboard input handling.
//
//   Tracks key-down, key-pressed, and key-released states per frame using scancode-indexed sets.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <SDL2/SDL.h>

#include <unordered_set>

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
	// Class: SDLKeyboard
	//
	// Description:
	//
	//   Wraps SDL2 keyboard input polling and state tracking.
	//
	//   - Maintains three scancode sets: keys currently held down, keys pressed this frame (leading edge), and keys
	//     released this frame (trailing edge).
	//
	//   - The endFrame method clears per-frame state for the next iteration.
	//
	//*****************************************************************************************************************

	class SDLKeyboard
	{
	public:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		// None

	private:

		std::unordered_set <SDL_Scancode> keysDown;
		std::unordered_set <SDL_Scancode> keysPressed;
		std::unordered_set <SDL_Scancode> keysReleased;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: isKeyDown
		//
		// Description:
		//
		//   Check whether a key is currently held down.
		//
		// Arguments:
		//
		//   key (SDL_Scancode):
		//     The SDL scancode of the key to query.
		//
		// Returns:
		//
		//   True if the key is currently held down, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool isKeyDown ( SDL_Scancode key )     const {
			return keysDown.count ( key ) > 0;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: isKeyPressed
		//
		// Description:
		//
		//   Check whether a key was pressed during the current frame (leading edge detection).
		//
		// Arguments:
		//
		//   key (SDL_Scancode):
		//     The SDL scancode of the key to query.
		//
		// Returns:
		//
		//   True if the key was pressed this frame, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool isKeyPressed ( SDL_Scancode key )  const {
			return keysPressed.count ( key ) > 0;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: isKeyReleased
		//
		// Description:
		//
		//   Check whether a key was released during the current frame (trailing edge detection).
		//
		// Arguments:
		//
		//   key (SDL_Scancode):
		//     The SDL scancode of the key to query.
		//
		// Returns:
		//
		//   True if the key was released this frame, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool isKeyReleased ( SDL_Scancode key ) const {
			return keysReleased.count ( key ) > 0;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: pollEvents
		//
		// Description:
		//
		//   Poll all pending SDL events, updating key-down, key-pressed, and key-released state.
		//
		//   Returns false if an SDL_QUIT event is received, signaling that the application should exit.
		//
		// Returns:
		//
		//   True if the application should continue running, false if a quit event was received.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool pollEvents ()
		{
			SDL_Event event;

			while ( SDL_PollEvent ( &event ) )
			{
				switch ( event.type )
				{
					case SDL_QUIT:
						return false;

					case SDL_KEYDOWN:
						if ( !event.key.repeat )
						{
							keysDown.insert ( event.key.keysym.scancode );
							keysPressed.insert ( event.key.keysym.scancode );
						}
						break;

					case SDL_KEYUP:
						keysDown.erase ( event.key.keysym.scancode );
						keysReleased.insert ( event.key.keysym.scancode );
						break;
				}
			}

			return true;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: endFrame
		//
		// Description:
		//
		//   Clear the per-frame key-pressed and key-released sets in preparation for the next frame's input polling.
		//
		//-------------------------------------------------------------------------------------------------------------

		void endFrame ()
		{
			keysPressed.clear ();
			keysReleased.clear ();
		}

	};
}
