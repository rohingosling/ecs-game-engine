//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Implementation of the SDLRenderer class's non-inline methods: initialization, shutdown, screen clearing,
//   primitive drawing (circles, lines, rectangles), texture loading and rendering, font loading, and text rendering.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "SDLRenderer.h"

#include <cmath>
#include <iostream>

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
	// Destructor: ~SDLRenderer
	//
	// Description:
	//
	//   Shut down the renderer, releasing all cached textures and fonts and quitting SDL_image and SDL_ttf.
	//
	//-----------------------------------------------------------------------------------------------------------------

	SDLRenderer::~SDLRenderer ()
	{
		shutdown ();
	}

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::init ( SDL_Renderer* renderer )
	{
		// Store the provided SDL_Renderer pointer and initialize SDL_image and SDL_ttf subsystems.

		sdlRenderer = renderer;

		// Initialize SDL_image for PNG support.
		// - If initialization fails, log the error but continue since texture loading will not work.

		if ( IMG_Init ( IMG_INIT_PNG ) == 0 )
		{
			std::cerr << "IMG_Init failed: " << IMG_GetError () << std::endl;
		}

		// Initialize SDL_ttf for font rendering.
		// - If initialization fails, log the error but continue since text rendering will not work.

		if ( TTF_Init () != 0 )
		{
			std::cerr << "TTF_Init failed: " << TTF_GetError () << std::endl;
		}

		SDL_SetRenderDrawBlendMode ( sdlRenderer, SDL_BLENDMODE_BLEND );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: shutdown
	//
	// Description:
	//
	//   Release all cached textures and fonts, then quit SDL_ttf and SDL_image subsystems.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::shutdown ()
	{
		// Destroy all cached textures and clear the cache.

		for ( auto& [path, texture] : textureCache )
		{
			SDL_DestroyTexture ( texture );
		}
		textureCache.clear ();

		// Close all cached fonts and clear the cache.

		for ( auto& [key, font] : fontCache )
		{
			TTF_CloseFont ( font );
		}
		fontCache.clear ();

		// Quit SDL_ttf and SDL_image subsystems.

		TTF_Quit ();
		IMG_Quit ();
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::clearScreen ( Color color )
	{
		SDL_SetRenderDrawColor ( sdlRenderer, color.r, color.g, color.b, color.a );
		SDL_RenderClear        ( sdlRenderer );
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawCircle ( int centerX, int centerY, int radius, Color color )
	{
		// Initialize the drawing color for the circle outline.

		SDL_SetRenderDrawColor ( sdlRenderer, color.r, color.g, color.b, color.a );

		// Initialize the midpoint circle algorithm variables.

		int x = radius;
		int y = 0;
		int d = 1 - radius;

		// Iterate over the points in the first octant and draw the corresponding points in all eight octants.

		while ( x >= y )
		{
			SDL_RenderDrawPoint ( sdlRenderer, centerX + x, centerY + y );
			SDL_RenderDrawPoint ( sdlRenderer, centerX - x, centerY + y );
			SDL_RenderDrawPoint ( sdlRenderer, centerX + x, centerY - y );
			SDL_RenderDrawPoint ( sdlRenderer, centerX - x, centerY - y );
			SDL_RenderDrawPoint ( sdlRenderer, centerX + y, centerY + x );
			SDL_RenderDrawPoint ( sdlRenderer, centerX - y, centerY + x );
			SDL_RenderDrawPoint ( sdlRenderer, centerX + y, centerY - x );
			SDL_RenderDrawPoint ( sdlRenderer, centerX - y, centerY - x );

			y++;

			// Update the decision variable based on the midpoint algorithm.

			if ( d < 0 )
			{
				// Midpoint is inside the circle, so choose the vertical step.

				d += 2 * y + 1;
			}
			else
			{
				// Midpoint is outside the circle, so choose the diagonal step.

				x--;
				d += 2 * ( y - x ) + 1;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawFilledCircle ( int centerX, int centerY, int radius, Color color )
	{
		// Initialize the drawing color for the circle fill.

		SDL_SetRenderDrawColor ( sdlRenderer, color.r, color.g, color.b, color.a );

		// Iterate over the vertical range of the circle and draw horizontal lines for each scanline that intersects the circle.

		for ( int dy = -radius; dy <= radius; dy++ )
		{
			int dx = static_cast< int >( std::sqrt ( radius * radius - dy * dy ) );
			SDL_RenderDrawLine ( sdlRenderer, centerX - dx, centerY + dy, centerX + dx, centerY + dy );
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawLine ( int lineStartX, int lineStartY, int lineEndX, int lineEndY, Color color, int thickness )
	{
		// Initialize the drawing color for the line.

		SDL_SetRenderDrawColor ( sdlRenderer, color.r, color.g, color.b, color.a );

		// Initially, draw a single-pixel line if thickness is 1 or less.

		if ( thickness <= 1 )
		{
			SDL_RenderDrawLine ( sdlRenderer, lineStartX, lineStartY, lineEndX, lineEndY );
			return;
		}

		// For thick lines, draw multiple parallel lines.

		double deltaX = static_cast< double >( lineEndX - lineStartX );
		double deltaY = static_cast< double >( lineEndY - lineStartY );
		double len    = std::sqrt ( deltaX * deltaX + deltaY * deltaY );

		// If the line is very short, just draw a single line to avoid division by zero and excessive offsets.

		if ( len < 0.001 ) return;

		// Compute the normalized perpendicular vector to the line for offsetting the parallel lines.

		double normalX = -deltaY / len;
		double normalY = deltaX / len;
		double half    = thickness / 2.0;

		// Draw multiple lines offset from the center line by increments of 1 pixel along the normal direction, covering the full thickness.

		for ( int i = 0; i < thickness; i++ )
		{
			double offset = -half + i + 0.5;
			int    ox     = static_cast< int > ( normalX * offset );
			int    oy     = static_cast< int > ( normalY * offset );

			SDL_RenderDrawLine
			(
				sdlRenderer, 
				lineStartX + ox, 
				lineStartY + oy, 
				lineEndX + ox, 
				lineEndY + oy
			);
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawRect ( int x, int y, int w, int h, Color color )
	{
		SDL_SetRenderDrawColor ( sdlRenderer, color.r, color.g, color.b, color.a );
		SDL_Rect rect = { x, y, w, h };
		SDL_RenderDrawRect ( sdlRenderer, &rect );
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawFilledRect ( int x, int y, int w, int h, Color color )
	{
		SDL_SetRenderDrawColor ( sdlRenderer, color.r, color.g, color.b, color.a );
		SDL_Rect rect = { x, y, w, h };
		SDL_RenderFillRect ( sdlRenderer, &rect );
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	SDL_Texture* SDLRenderer::loadTexture ( const std::string& path )
	{
		// Check if the texture is already cached. If so, return it.

		auto it = textureCache.find ( path );
		if ( it != textureCache.end () )
		{
			return it->second;
		}

		// Load the texture from the specified file path using SDL_image. If loading fails, log the error and return nullptr.

		SDL_Texture* texture = IMG_LoadTexture ( sdlRenderer, path.c_str () );
		if ( !texture )
		{
			std::cerr << "Failed to load texture: " << path << " - " << IMG_GetError () << std::endl;
			return nullptr;
		}

		// Set the texture blend mode to enable alpha blending, then cache and return the loaded texture.

		SDL_SetTextureBlendMode ( texture, SDL_BLENDMODE_BLEND );
		textureCache [ path ] = texture;
		return texture;
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawTexture ( SDL_Texture* texture, int x, int y, int w, int h, double opacity )
	{
		// If the texture is null, do not attempt to render and simply return.

		if ( !texture ) return;

		// Set the texture alpha modulation based on the specified opacity multiplier, then render the texture to the destination rectangle.

		SDL_SetTextureAlphaMod ( texture, static_cast < uint8_t > ( opacity * 255.0 ) );

		// Define the destination rectangle for rendering the texture at the specified position and size.

		SDL_Rect dest = { x, y, w, h };
		SDL_RenderCopy ( sdlRenderer, texture, nullptr, &dest );

		// Reset the texture alpha modulation to fully opaque after rendering to avoid affecting subsequent renders of the same texture.

		SDL_SetTextureAlphaMod ( texture, 255 );
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	TTF_Font* SDLRenderer::loadFont ( const std::string& path, int size )
	{
		// Create a unique cache key by combining the font path and size, then check if the font is already cached. If so, return it.

		std::string key = path + ":" + std::to_string ( size );

		// If the font is not cached, attempt to load it from the specified file path and size using SDL_ttf. If loading fails, log the error and return nullptr.

		auto it = fontCache.find ( key );
		if ( it != fontCache.end () )
		{
			return it->second;
		}

		// Load the font from the specified file path and size using SDL_ttf. If loading fails, log the error and return nullptr.

		TTF_Font* font = TTF_OpenFont ( path.c_str (), size );
		if ( !font )
		{
			std::cerr << "Failed to load font: " << path << " - " << TTF_GetError () << std::endl;
			return nullptr;
		}

		// Cache the loaded font using the combined path:size key, then return the font pointer.

		fontCache [ key ] = font;
		return font;
	}

	//-----------------------------------------------------------------------------------------------------------------
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
	//-----------------------------------------------------------------------------------------------------------------

	void SDLRenderer::drawText ( const std::string& text, int x, int y, TTF_Font* font, Color color, double opacity )
	{
		// If the font is null or the text is empty, do not attempt to render and simply return.

		if ( !font || text.empty () ) return;

		SDL_Color sdlColor = { color.r, color.g, color.b, static_cast< uint8_t >( color.a * opacity ) };
		SDL_Surface* surface = TTF_RenderUTF8_Blended ( font, text.c_str (), sdlColor );

		// If surface creation failed, log the error and return without attempting to create a texture or render.

		if ( !surface ) return;

		SDL_Texture* texture = SDL_CreateTextureFromSurface ( sdlRenderer, surface );
		SDL_FreeSurface ( surface );

		// If texture creation failed, log the error and return without attempting to render. 

		if ( !texture ) return;

		SDL_SetTextureBlendMode ( texture, SDL_BLENDMODE_BLEND );

		SDL_Rect dest = { x, y, 0, 0 };
		
		SDL_QueryTexture   ( texture, nullptr, nullptr, &dest.w, &dest.h );
		SDL_RenderCopy     ( sdlRenderer, texture, nullptr, &dest );
		SDL_DestroyTexture ( texture );
	}
}
