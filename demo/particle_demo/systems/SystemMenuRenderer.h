//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemMenuRenderer class, an ECS system that renders the menu UI including background
//   images, button shadows, button images, button text labels, and text box content.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../ecs/System.h"
#include "../../../ecs/World.h"
#include "../../../engine/platform/SDLRenderer.h"
#include "../../../engine/ApplicationSettings.h"
#include "../components/ComponentButtonState.h"
#include "../components/ComponentButtonImage.h"
#include "../components/ComponentButtonText.h"
#include "../components/ComponentRectangle.h"
#include "../components/ComponentBackgroundImage.h"
#include "../components/ComponentTextBox.h"

#include <SDL2/SDL.h>
#include <string>

//*********************************************************************************************************************
// Class: SystemMenuRenderer
//
// Description:
//
//   An ECS system that renders the menu screens using a multi-pass approach.
//
//   - Draws layers in order: background image, button drop shadows, state-dependent button images, centered
//     button text labels, and multi-line text boxes for content screens.
//
//   - Uses SDL2 textures and fonts via the SDLRenderer facade.
//
//*********************************************************************************************************************

class SystemMenuRenderer : public ecs::System
{
public:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	// Pointers to shared resources and configuration values for rendering.

	engine::SDLRenderer*         renderer     = nullptr;
	engine::ApplicationSettings* settings     = nullptr;
	int                          screenWidth  = 1920;
	int                          screenHeight = 1080;

	// Entity slots for menu elements. Background image is a single entity, while buttons and text boxes are stored in vectors

	ecs::Entity backgroundEntity = ecs::NULL_ENTITY;

	// Button entities are expected to have components defining their state, images, text, and layout rectangle. Text box entities

	std::vector <ecs::Entity> buttonEntities;
	std::vector <ecs::Entity> textBoxEntities;

	std::string buttonFontPath;
	std::string textBoxFontPath;

	// Shadow rendering parameters for button drop shadows.

	int    shadowOffsetX = 6;
	int    shadowOffsetY = 6;
	double shadowOpacity = 0.1;

	// Selected button text color, applied to the text of any button with the selected flag set in its ComponentButtonState.	

	int selectedColorR = 255;
	int selectedColorG = 255;
	int selectedColorB = 255;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Execute the multi-pass menu rendering pipeline for the current frame.
	//
	//   Draws the background, button shadows, button images, button text labels, and text box content in
	//   sequential render passes.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//   dt (double):
	//     Delta time in seconds since the previous frame. Unused by this system.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void update ( ecs::World& world, double dt ) override
	{
		// Early-out if the renderer has not been assigned; nothing can be drawn without it.

		if ( !renderer ) return;

		// Pass 1: Background image.

		if ( backgroundEntity != ecs::NULL_ENTITY && world.hasComponent <ComponentBackgroundImage> ( backgroundEntity ) )
		{
			// Retrieve the background image component and load its texture from disk (or cache).

			auto& background = world.getComponent <ComponentBackgroundImage> ( backgroundEntity );
			SDL_Texture* backgroundTexture = renderer->loadTexture ( background.imagePath );

			// Draw the background texture stretched to fill the screen, or fall back to a solid black clear.

			if ( backgroundTexture )
			{
				renderer->drawTexture ( backgroundTexture, 0, 0, screenWidth, screenHeight );
			}
			else
			{
				renderer->clearScreen ( { 0, 0, 0, 255 } );
			}
		}
		else
		{
			renderer->clearScreen ( { 0, 0, 0, 255 } );
		}

		// Pass 2: Button shadows.

		for ( auto entity : buttonEntities )
		{
			// Skip entities that lack a button state component, as they are not renderable buttons.

			if ( !world.hasComponent <ComponentButtonState> ( entity ) ) continue;

			// Skip disabled buttons so that inactive screens sharing the same entity slots are not drawn.

			auto& state = world.getComponent <ComponentButtonState> ( entity );
			if ( !state.enabled ) continue;

			// Fetch the button image paths and the rectangle defining the button's on-screen position.

			auto& buttonImage = world.getComponent <ComponentButtonImage> ( entity );
			auto& rect        = world.getComponent <ComponentRectangle> ( entity );

			SDL_Texture* shadowTexture = renderer->loadTexture ( buttonImage.imageShadow );
			if ( !shadowTexture ) continue;

			// Query the shadow texture dimensions, offset the position by the shadow offset, and draw at reduced opacity.

			int w = 0, h = 0;
			SDL_QueryTexture ( shadowTexture, nullptr, nullptr, &w, &h );

			int x = static_cast <int> ( rect.origin.x ) + shadowOffsetX;
			int y = static_cast <int> ( rect.origin.y ) + shadowOffsetY;

			renderer->drawTexture ( shadowTexture, x, y, w, h, shadowOpacity );
		}

		// Pass 3: Button images (state-dependent).

		for ( auto entity : buttonEntities )
		{
			// Guard: ensure this entity has a button state before attempting to read button components.

			if ( !world.hasComponent <ComponentButtonState> ( entity ) ) continue;

			// Skip disabled buttons entirely to prevent drawing buttons belonging to inactive menu screens.

			auto& state = world.getComponent <ComponentButtonState> ( entity );
			if ( !state.enabled ) continue;

			// Retrieve the button image set and the on-screen rectangle for positioning.

			auto& buttonImage = world.getComponent <ComponentButtonImage> ( entity );
			auto& rect        = world.getComponent <ComponentRectangle> ( entity );

			// Select the appropriate button image based on pressed and selected state flags.

			std::string imagePath;

			if ( state.pressed && state.selected )
			{
				imagePath = buttonImage.imageDownSelected;
			}
			else if ( state.selected )
			{
				imagePath = buttonImage.imageUpSelected;
			}
			else
			{
				imagePath = buttonImage.imageUp;
			}

			// Load the chosen button texture from the image path, skipping this button if loading fails.

			SDL_Texture* texture = renderer->loadTexture ( imagePath );
			if ( !texture ) continue;

			// Query the native texture dimensions to preserve the button image aspect ratio during rendering.

			int w = 0, h = 0;
			SDL_QueryTexture ( texture, nullptr, nullptr, &w, &h );

			// Convert the rectangle origin to integer screen coordinates and draw the button texture.

			int x = static_cast <int> ( rect.origin.x );
			int y = static_cast <int> ( rect.origin.y );

			renderer->drawTexture ( texture, x, y, w, h );
		}

		// Pass 4: Button text.

		for ( auto entity : buttonEntities )
		{
			// Verify the entity has a button state component before accessing other button components.

			if ( !world.hasComponent <ComponentButtonState> ( entity ) ) continue;

			// Skip disabled buttons to avoid rendering text for buttons on inactive menu screens.

			auto& state = world.getComponent <ComponentButtonState> ( entity );
			if ( !state.enabled ) continue;

			// Fetch the text label content, button image set, and rectangle for layout calculations.

			auto& buttonText  = world.getComponent <ComponentButtonText>  ( entity );
			auto& buttonImage = world.getComponent <ComponentButtonImage> ( entity );
			auto& rect        = world.getComponent <ComponentRectangle>   ( entity );

			// Determine text color.

			engine::Color textColor;

			if ( state.selected )
			{
				textColor =
				{
					static_cast <uint8_t> ( selectedColorR ),
					static_cast <uint8_t> ( selectedColorG ),
					static_cast <uint8_t> ( selectedColorB ),
					255
				};
			}
			else
			{
				textColor =
				{
					static_cast <uint8_t> ( buttonText.colorR ),
					static_cast <uint8_t> ( buttonText.colorG ),
					static_cast <uint8_t> ( buttonText.colorB ),
					255
				};
			}

			// Load the button label font at the size specified by the button text component.

			TTF_Font* font = renderer->loadFont ( buttonFontPath, buttonText.size );
			if ( !font ) continue;

			// Get button image dimensions for centering.

			SDL_Texture* buttonTexture = renderer->loadTexture ( buttonImage.imageUp );
			int buttonWidth            = 600;
			int buttonHeight           = 60;

			// If the button texture loaded successfully, query its actual pixel dimensions for precise centering.

			if ( buttonTexture )
			{
				SDL_QueryTexture ( buttonTexture, nullptr, nullptr, &buttonWidth, &buttonHeight );
			}

			// Convert the button rectangle origin to integer screen coordinates for text placement.

			int bx = static_cast <int> ( rect.origin.x );
			int by = static_cast <int> ( rect.origin.y );

			// Measure the rendered pixel width and height of the button label string for centering calculations.

			int textW = 0, textH = 0;
			TTF_SizeUTF8 ( font, buttonText.text.c_str (), &textW, &textH );

			// Center the text label horizontally and vertically within the button image bounds.

			int textX = bx + ( buttonWidth - textW ) / 2;
			int textY = by + ( buttonHeight - textH ) / 2;

			// Render the button label text at the computed centered position using the determined color.

			renderer->drawText ( buttonText.text, textX, textY, font, textColor );
		}

		// Pass 5: Text boxes.

		for ( auto entity : textBoxEntities )
		{
			if ( !world.hasComponent <ComponentTextBox> ( entity ) ) continue;

			// Check if the entity is enabled (via ComponentButtonState if present, otherwise always render).

			if ( world.hasComponent <ComponentButtonState> ( entity ) )
			{
				auto& state = world.getComponent <ComponentButtonState> ( entity );
				if ( !state.enabled ) continue;
			}

			// Retrieve the text box component and load the text box font at the configured size.

			auto& tb = world.getComponent <ComponentTextBox> ( entity );

			TTF_Font* font = renderer->loadFont ( textBoxFontPath, tb.fontSize );
			if ( !font ) continue;

			engine::Color textColor =
			{
				static_cast <uint8_t> ( tb.colorR ),
				static_cast <uint8_t> ( tb.colorG ),
				static_cast <uint8_t> ( tb.colorB ),
				255
			};

			// Compute the font line height and starting draw position from the text box configuration.

			int lineHeight = TTF_FontLineSkip  ( font );
			int x          = static_cast <int> ( tb.position.x );
			int y          = static_cast <int> ( tb.position.y );

			// Split text into lines and render.

			std::string remaining = tb.text;
			std::size_t pos = 0;

			// Iterate through the text, splitting on newline characters and rendering each line individually.

			while ( ( pos = remaining.find ( '\n' ) ) != std::string::npos || !remaining.empty () )
			{
				// Extract the next line up to the newline delimiter, or consume the entire remaining string.

				std::string line;

				if ( pos != std::string::npos )
				{
					line = remaining.substr ( 0, pos );
					remaining = remaining.substr ( pos + 1 );
				}
				else
				{
					line = remaining;
					remaining.clear ();
				}

				if ( line.empty () )
				{
					y += lineHeight;
					continue;
				}

				// Measure the rendered pixel width and height of the current line for alignment calculations.

				int lineW = 0, lineH = 0;
				TTF_SizeUTF8 ( font, line.c_str (), &lineW, &lineH );

				int drawX;

				// Compute the horizontal draw position based on the text box alignment mode (left, center, or right).

				switch ( tb.align )
				{
					case 1:  drawX = x + ( tb.width - lineW ) / 2; break;  // center
					case 2:  drawX = x + tb.width - lineW;       break;  // right
					default: drawX = x;                           break;  // left
				}

				// Render the current line at the aligned position with the configured text color and opacity, then advance y.

				renderer->drawText ( line, drawX, y, font, textColor, tb.textOpacity );
				y += lineHeight;
			}
		}
	}
};
