//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SDLWindow class, which wraps SDL2 window and renderer creation, destruction, and buffer
//   presentation behind a simple RAII interface.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <SDL2/SDL.h>
#include <string>

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
	// Class: SDLWindow
	//
	// Description:
	//
	//   Manages the lifecycle of an SDL2 window and its associated hardware-accelerated renderer.
	//
	//   - Provides methods to create, destroy, and present the window, as well as accessors for the underlying SDL
	//     handles and dimensions.
	//
	//   - On Windows, applies a dark title bar theme via the DWM API.
	//
	//*****************************************************************************************************************

	class SDLWindow
	{
	public:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		// None

	private:

		SDL_Window*   window   = nullptr;
		SDL_Renderer* renderer = nullptr;
		int           width    = 0;
		int           height   = 0;

	public:

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/1: SDLWindow
		//
		// Description:
		//
		//   Default constructor.
		//
		//   Initializes all pointers to nullptr and dimensions to zero.
		//
		//-------------------------------------------------------------------------------------------------------------

		SDLWindow () = default;

		//=============================================================================================================
		// Destructor
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Destructor: ~SDLWindow
		//
		// Description:
		//
		//   Destroy the SDL renderer and window if they are still active, releasing all associated SDL resources.
		//
		//-------------------------------------------------------------------------------------------------------------

		~SDLWindow ();

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getWindow
		//
		// Description:
		//
		//   Return the raw SDL_Window pointer for direct SDL2 API access.
		//
		// Returns:
		//
		//   A pointer to the underlying SDL_Window, or nullptr if not created.
		//
		//-------------------------------------------------------------------------------------------------------------

		SDL_Window* getWindow () const
		{
			return window;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getRenderer
		//
		// Description:
		//
		//   Return the raw SDL_Renderer pointer for direct SDL2 rendering API access.
		//
		// Returns:
		//
		//   A pointer to the underlying SDL_Renderer, or nullptr if not created.
		//
		//-------------------------------------------------------------------------------------------------------------

		SDL_Renderer* getRenderer () const
		{
			return renderer;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getWidth
		//
		// Description:
		//
		//   Return the width of the window in pixels.
		//
		// Returns:
		//
		//   The window width in pixels.
		//
		//-------------------------------------------------------------------------------------------------------------

		int getWidth () const
		{
			return width;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getHeight
		//
		// Description:
		//
		//   Return the height of the window in pixels.
		//
		// Returns:
		//
		//   The window height in pixels.
		//
		//-------------------------------------------------------------------------------------------------------------

		int getHeight () const
		{
			return height;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: create
		//
		// Description:
		//
		//   Initialize SDL video, create a centered window with the specified title and dimensions, and create a
		//   hardware-accelerated renderer with VSync enabled.
		//
		//   Applies a dark title bar on Windows.
		//
		// Arguments:
		//
		//   title (const std::string&):
		//     The text displayed in the window title bar.
		//
		//   width (int):
		//     The width of the window in pixels.
		//
		//   height (int):
		//     The height of the window in pixels.
		//
		// Returns:
		//
		//   True if the window and renderer were created successfully, false on failure.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool create ( const std::string& title, int width, int height );

		//-------------------------------------------------------------------------------------------------------------
		// Method: destroy
		//
		// Description:
		//
		//   Destroy the SDL renderer and window, setting both pointers to nullptr.
		//
		//-------------------------------------------------------------------------------------------------------------

		void destroy ();

		//-------------------------------------------------------------------------------------------------------------
		// Method: present
		//
		// Description:
		//
		//   Present the current render buffer to the screen by calling SDL_RenderPresent on the internal renderer.
		//
		//-------------------------------------------------------------------------------------------------------------

		void present ();

	private:

		//-------------------------------------------------------------------------------------------------------------
		// Method: enableDarkTitleBar
		//
		// Description:
		//
		//   Apply a dark title bar theme on Windows using the DWM API.
		//
		//   No-op on other platforms.
		//
		//-------------------------------------------------------------------------------------------------------------

		void enableDarkTitleBar ();
	};
}
