//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Color struct and SDLRenderer class, providing a high-level rendering facade over SDL2, SDL_image,
//   and SDL_ttf.
//
//   Supports primitive drawing, texture loading and rendering, and TrueType font text rendering.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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
	// Struct: Color
	//
	// Description:
	//
	//   Represents an RGBA color with 8-bit unsigned components.
	//
	//   - Defaults to opaque white (255, 255, 255, 255).
	//
	//   - Used throughout the rendering API to specify draw and fill colors.
	//
	//*****************************************************************************************************************

	struct Color
	{
		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		uint8_t r = 255;
		uint8_t g = 255;
		uint8_t b = 255;
		uint8_t a = 255;
	};

	//*****************************************************************************************************************
	// Class: SDLRenderer
	//
	// Description:
	//
	//   High-level rendering facade wrapping SDL2, SDL_image, and SDL_ttf.
	//
	//   - Provides methods for screen clearing, primitive drawing (lines, rectangles, circles), texture loading and
	//     rendering with opacity, and TrueType font text rendering.
	//
	//   - Manages internal caches for loaded textures and fonts.
	//
	//*****************************************************************************************************************

	class SDLRenderer
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		SDL_Renderer*                                  sdlRenderer  = nullptr;
		std::unordered_map <std::string, SDL_Texture*> textureCache = {};
		std::unordered_map <std::string, TTF_Font*>    fontCache    = {};

	public:

		//=============================================================================================================
		// Value Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: getSDLRenderer
		//
		// Description:
		//
		//   Return the raw SDL_Renderer pointer for direct SDL2 rendering API access.
		//
		// Returns:
		//
		//   A pointer to the underlying SDL_Renderer, or nullptr if not initialized.
		//
		//-------------------------------------------------------------------------------------------------------------

		SDL_Renderer* getSDLRenderer () const
		{
			return sdlRenderer;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/1: SDLRenderer
		//
		// Description:
		//
		//   Default constructor.
		//
		//   Initializes the internal SDL renderer pointer to nullptr.
		//
		//-------------------------------------------------------------------------------------------------------------

		SDLRenderer () = default;

		//=============================================================================================================
		// Destructor
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Destructor: ~SDLRenderer
		//
		// Description:
		//
		//   Shut down the renderer, releasing all cached textures and fonts and quitting SDL_image and SDL_ttf.
		//
		//-------------------------------------------------------------------------------------------------------------

		~SDLRenderer ();

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: init
		//
		// Description:
		//
		//   Initialize the renderer with an existing SDL_Renderer, initializing SDL_image for PNG support and
		//   SDL_ttf for font rendering, and enabling alpha blending.
		//
		// Arguments:
		//
		//   renderer (SDL_Renderer*):
		//     A valid SDL_Renderer obtained from an SDLWindow or SDL_CreateRenderer call.
		//
		//-------------------------------------------------------------------------------------------------------------

		void init ( SDL_Renderer* renderer );

		//-------------------------------------------------------------------------------------------------------------
		// Method: shutdown
		//
		// Description:
		//
		//   Release all cached textures and fonts, then quit SDL_ttf and SDL_image subsystems.
		//
		//-------------------------------------------------------------------------------------------------------------

		void shutdown ();

		//-------------------------------------------------------------------------------------------------------------
		// Method: clearScreen
		//
		// Description:
		//
		//   Clear the entire render target to the specified color.
		//
		// Arguments:
		//
		//   color (Color):
		//     The RGBA color to fill the screen with. Defaults to opaque black.
		//
		//-------------------------------------------------------------------------------------------------------------

		void clearScreen ( Color color = { 0, 0, 0, 255 } );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawCircle
		//
		// Description:
		//
		//   Draw an unfilled circle outline using the midpoint circle algorithm.
		//
		// Arguments:
		//
		//   cx (int):
		//     The x coordinate of the circle center in pixels.
		//
		//   cy (int):
		//     The y coordinate of the circle center in pixels.
		//
		//   radius (int):
		//     The radius of the circle in pixels.
		//
		//   color (Color):
		//     The RGBA color of the circle outline.
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawCircle ( int centerX, int centerY, int radius, Color color );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawFilledCircle
		//
		// Description:
		//
		//   Draw a filled circle by rendering horizontal scanlines for each row within the circle's bounding box.
		//
		// Arguments:
		//
		//   cx (int):
		//     The x coordinate of the circle center in pixels.
		//
		//   cy (int):
		//     The y coordinate of the circle center in pixels.
		//
		//   radius (int):
		//     The radius of the circle in pixels.
		//
		//   color (Color):
		//     The RGBA fill color.
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawFilledCircle ( int centerX, int centerY, int radius, Color color );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawLine
		//
		// Description:
		//
		//   Draw a line between two points with optional thickness.
		//
		//   Single-pixel lines use SDL_RenderDrawLine; thicker lines are drawn as multiple parallel lines offset along
		//   the perpendicular.
		//
		// Arguments:
		//
		//   x1 (int):
		//     The x coordinate of the start point.
		//
		//   y1 (int):
		//     The y coordinate of the start point.
		//
		//   x2 (int):
		//     The x coordinate of the end point.
		//
		//   y2 (int):
		//     The y coordinate of the end point.
		//
		//   color (Color):
		//     The RGBA color of the line.
		//
		//   thickness (int):
		//     The line width in pixels. Defaults to 1.
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawLine ( int lineStartX, int lineStartY, int lineEndX, int lineEndY, Color color, int thickness = 1 );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawRect
		//
		// Description:
		//
		//   Draw an unfilled rectangle outline at the specified position and dimensions.
		//
		// Arguments:
		//
		//   x (int):
		//     The x coordinate of the top-left corner.
		//
		//   y (int):
		//     The y coordinate of the top-left corner.
		//
		//   w (int):
		//     The width of the rectangle in pixels.
		//
		//   h (int):
		//     The height of the rectangle in pixels.
		//
		//   color (Color):
		//     The RGBA color of the rectangle outline.
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawRect ( int x, int y, int w, int h, Color color );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawFilledRect
		//
		// Description:
		//
		//   Draw a filled rectangle at the specified position and dimensions.
		//
		// Arguments:
		//
		//   x (int):
		//     The x coordinate of the top-left corner.
		//
		//   y (int):
		//     The y coordinate of the top-left corner.
		//
		//   w (int):
		//     The width of the rectangle in pixels.
		//
		//   h (int):
		//     The height of the rectangle in pixels.
		//
		//   color (Color):
		//     The RGBA fill color.
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawFilledRect ( int x, int y, int w, int h, Color color );

		//-------------------------------------------------------------------------------------------------------------
		// Method: loadTexture
		//
		// Description:
		//
		//   Load an image file as an SDL texture, caching it by path.
		//
		//   Subsequent calls with the same path return the cached texture without reloading from disk.
		//
		// Arguments:
		//
		//   path (const std::string&):
		//     The file system path to the image file.
		//
		// Returns:
		//
		//   A pointer to the loaded SDL_Texture, or nullptr on failure.
		//
		//-------------------------------------------------------------------------------------------------------------

		SDL_Texture* loadTexture ( const std::string& path );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawTexture
		//
		// Description:
		//
		//   Render a texture to the screen at the specified position and size with optional opacity.
		//
		// Arguments:
		//
		//   texture (SDL_Texture*):
		//     The texture to render.
		//
		//   x (int):
		//     The x coordinate of the destination top-left corner.
		//
		//   y (int):
		//     The y coordinate of the destination top-left corner.
		//
		//   w (int):
		//     The width of the destination rectangle in pixels.
		//
		//   h (int):
		//     The height of the destination rectangle in pixels.
		//
		//   opacity (double):
		//     The opacity multiplier in the range [0.0, 1.0]. Defaults to 1.0 (fully opaque).
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawTexture ( SDL_Texture* texture, int x, int y, int w, int h, double opacity = 1.0 );

		//-------------------------------------------------------------------------------------------------------------
		// Method: loadFont
		//
		// Description:
		//
		//   Load a TrueType font file at the specified point size, caching it by a combined path:size key.
		//
		//   Subsequent calls with the same path and size return the cached font.
		//
		// Arguments:
		//
		//   path (const std::string&):
		//     The file system path to the TTF font file.
		//
		//   size (int):
		//     The point size at which to load the font.
		//
		// Returns:
		//
		//   A pointer to the loaded TTF_Font, or nullptr on failure.
		//
		//-------------------------------------------------------------------------------------------------------------

		TTF_Font* loadFont ( const std::string& path, int size );

		//-------------------------------------------------------------------------------------------------------------
		// Method: drawText
		//
		// Description:
		//
		//   Render a UTF-8 text string to the screen at the specified position using a loaded font, color, and
		//   optional opacity.
		//
		//   The text surface is created, converted to a texture, rendered, and immediately freed.
		//
		// Arguments:
		//
		//   text (const std::string&):
		//     The UTF-8 text string to render.
		//
		//   x (int):
		//     The x coordinate of the text top-left corner.
		//
		//   y (int):
		//     The y coordinate of the text top-left corner.
		//
		//   font (TTF_Font*):
		//     The TrueType font to use for rendering.
		//
		//   color (Color):
		//     The RGBA color of the text.
		//
		//   opacity (double):
		//     The opacity multiplier in the range [0.0, 1.0]. Defaults to 1.0.
		//
		//-------------------------------------------------------------------------------------------------------------

		void drawText ( const std::string& text, int x, int y, TTF_Font* font, Color color, double opacity = 1.0 );
	};
}
