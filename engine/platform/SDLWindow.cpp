//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Implementation of the SDLWindow class's non-inline methods: window and renderer creation, destruction, buffer
//   presentation, and Windows dark title bar integration.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "SDLWindow.h"

#include <iostream>

#ifdef _WIN32
#include <dwmapi.h>
#include <SDL2/SDL_syswm.h>
#pragma comment(lib, "dwmapi.lib")
#endif

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
	//=================================================================================================================
	// Destructor
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Destructor: ~SDLWindow
	//
	// Description:
	//
	//   Destroy the SDL renderer and window if they are still active, releasing all associated SDL resources.
	//
	//-----------------------------------------------------------------------------------------------------------------

	SDLWindow::~SDLWindow ()
	{
		destroy ();
	}

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
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
	//   w (int):
	//     The width of the window in pixels.
	//
	//   h (int):
	//     The height of the window in pixels.
	//
	// Returns:
	//
	//   True if the window and renderer were created successfully, false on failure.
	//
	//-----------------------------------------------------------------------------------------------------------------

	bool SDLWindow::create ( const std::string& title, int w, int h )
	{
		// Initialize dimensions and SDL video subsystem

		width  = w;
		height = h;

		// Initialize SDL video subsystem and create window and renderer, checking for errors at each step.

		if ( SDL_Init ( SDL_INIT_VIDEO ) != 0 )
		{
			std::cerr << "SDL_Init failed: " << SDL_GetError () << std::endl;
			return false;
		}

		// Create a centered window with the specified title and dimensions, and a hardware-accelerated renderer with VSync enabled.

		window = SDL_CreateWindow
		(
			title.c_str (),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width, height,
			SDL_WINDOW_SHOWN
		);

		// Check for errors in window and renderer creation, printing SDL error messages to stderr if failures occur.

		if ( !window )
		{
			std::cerr << "SDL_CreateWindow failed: " << SDL_GetError () << std::endl;
			return false;
		}

		// Use hardware acceleration and VSync for smooth rendering performance, falling back to software rendering if necessary.

		renderer = SDL_CreateRenderer
		(
			window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);

		// If hardware acceleration is unavailable, fall back to software rendering and print a warning.

		if ( !renderer )
		{
			std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError () << std::endl;
			return false;
		}

		// Apply a dark title bar on Windows for better aesthetics.

		enableDarkTitleBar ();

		// Window and renderer created successfully, return true.

		return true;
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: destroy
	//
	// Description:
	//
	//   Destroy the SDL renderer and window, setting both pointers to nullptr.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void SDLWindow::destroy ()
	{
		// Destroy the renderer and window if they exist, ensuring all SDL resources are released properly.

		if ( renderer )
		{
			SDL_DestroyRenderer ( renderer );
			renderer = nullptr;
		}

		// Destroy the window if it exists, ensuring all SDL resources are released properly.

		if ( window )
		{
			SDL_DestroyWindow ( window );
			window = nullptr;
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: present
	//
	// Description:
	//
	//   Present the current render buffer to the screen by calling SDL_RenderPresent on the internal renderer.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void SDLWindow::present ()
	{
		// Present the current render buffer to the screen if the renderer exists, ensuring that rendering operations are visible.

		if ( renderer )
		{
			SDL_RenderPresent ( renderer );
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: enableDarkTitleBar
	//
	// Description:
	//
	//   Apply a dark title bar theme on Windows using the DWM API.
	//
	//   No-op on other platforms.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void SDLWindow::enableDarkTitleBar ()
	{
		// For Windows, use the DWM API to set the window attribute for dark mode, enhancing the visual integration 
		// of the SDL window with modern Windows themes.
		//
		// - For other operating systems, this method does nothing, as dark title bar support is not implemented or 
		//   may not be applicable.

#ifdef _WIN32

		// Retrieve the native window handle from the SDL_Window using SDL_SysWMinfo, and set the DWM window attribute for dark mode.

		SDL_SysWMinfo wmInfo;
		SDL_VERSION ( &wmInfo.version );

		// Check if we successfully retrieved the window information, and if so, set the dark mode attribute using DwmSetWindowAttribute.

		if ( SDL_GetWindowWMInfo ( window, &wmInfo ) )
		{
			HWND hwnd        = wmInfo.info.win.window;
			BOOL useDarkMode = TRUE;

			DwmSetWindowAttribute ( hwnd, 20, &useDarkMode, sizeof ( useDarkMode ) );
		}
#endif
	}
}
