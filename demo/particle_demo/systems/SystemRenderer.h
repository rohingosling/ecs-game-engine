//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemRenderer class, an ECS system that performs multi-pass rendering of the particle
//   simulation including background, trails, shadows, sprites, wireframe overlays, HUD, and pause indicator.
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
#include "../components/ComponentParticleGroup.h"
#include "../components/ComponentTransform.h"
#include "../components/ComponentPhysics.h"
#include "../components/ComponentCircle.h"
#include "../components/ComponentSprite.h"
#include "../components/ComponentShadow.h"
#include "../components/ComponentTrail.h"
#include "../components/ComponentProjection2D.h"
#include "../components/ComponentBackgroundImage.h"
#include "../components/ComponentUserControl.h"
#include "../components/ComponentHud.h"
#include "../components/ComponentWorld.h"

#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>

//*********************************************************************************************************************
// Class: SystemRenderer
//
// Description:
//
//   An ECS system that performs multi-pass rendering of the particle simulation scene.
//
//   - Draws layers in order: 1. Background image, 2. Motion trails, 3. Drop shadows, 4. Particle sprites,
//     5. Wireframe circle overlays, 6. HUD text overlay, 7. Centered pause indicator.
//
//   - Uses SDL2 textures and fonts via the SDLRenderer facade.
//
//*********************************************************************************************************************

class SystemRenderer : public ecs::System
{
public:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::SDLRenderer* renderer     = nullptr;
	ecs::Entity          worldEntity  = ecs::NULL_ENTITY;
	ecs::Entity          hudEntity    = ecs::NULL_ENTITY;
	int                  screenWidth  = 1920;
	int                  screenHeight = 1080;
	std::string          hudFontPath;
	std::string          pauseFontPath;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Execute the multi-pass rendering pipeline for the current frame.
	//
	//   Draws the background, particle trails, shadows, sprites, wireframe circles, HUD overlay, and pause
	//   indicator in sequential render passes.
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
		// Early out if the renderer or world entity has not been assigned by the owning engine.

		if ( !renderer || worldEntity == ecs::NULL_ENTITY ) return;

		auto& worldComponent = world.getComponent <ComponentWorld> ( worldEntity );

		// Compute the world-to-screen scaling factor based on the screen height and zoom level.

		double zoom          = 1.0;
		double worldToScreen = screenHeight * zoom;

		// Pass 1: Background.

		if ( world.hasComponent <ComponentBackgroundImage> ( worldEntity ) )
		{
			// Retrieve the background image component and load its texture from the cached resource path.

			auto& background = world.getComponent <ComponentBackgroundImage> ( worldEntity );
			SDL_Texture* backgroundTexture = renderer->loadTexture ( background.imagePath );

			// Draw the background texture stretched to fill the screen, or clear to black if the texture failed to load.

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
			// No background image component present on the world entity, so clear to solid black as the default backdrop.

			renderer->clearScreen ( { 0, 0, 0, 255 } );
		}

		// Pass 2: Trails.

		if ( worldComponent.trailsVisible )
		{
			// Iterate over all particle entities and render their motion trail line segments with fading opacity.

			for ( auto entity : entities )
			{
				// Fetch the trail history and projection scale for this particle entity.

				auto& trail      = world.getComponent <ComponentTrail>        ( entity );
				auto& projection = world.getComponent <ComponentProjection2D> ( entity );

				// Compute the per-entity world-to-screen scale from the projection and count the trail history points.

				double z                  = projection.scale.x;
				double worldToScreenScale = screenHeight * z;

				int totalPoints = static_cast <int> ( trail.history.size () );

				// A trail needs at least two points to form a line segment; skip entities with insufficient history.

				if ( totalPoints < 2 ) continue;

				// Draw each consecutive pair of trail points as a line segment with interpolated opacity from tail to head.

				for ( int i = 0; i < totalPoints - 1; ++i )
				{
					// Linearly interpolate opacity from the tail value to the head value based on the segment's position in the trail.

					double progress = static_cast <double> ( i ) / ( totalPoints - 1 );
					double alpha    = trail.opacityTail + progress * ( trail.opacityHead - trail.opacityTail );

					// Convert the normalized alpha to a byte value and build the RGBA color for this trail segment.

					uint8_t a = static_cast <uint8_t> ( alpha * 255.0 );

					engine::Color color =
					{
						static_cast <uint8_t> ( trail.colorR ),
						static_cast <uint8_t> ( trail.colorG ),
						static_cast <uint8_t> ( trail.colorB ),
						a
					};

					// Retrieve the start and end positions of the current trail line segment from the history buffer.

					auto& p1 = trail.history [ i ];
					auto& p2 = trail.history [ i + 1 ];

					// Transform the trail segment endpoints from world coordinates to screen pixel coordinates.

					int lineStartX = static_cast <int> ( p1.x * worldToScreenScale );
					int lineStartY = static_cast <int> ( p1.y * worldToScreenScale );
					int lineEndX   = static_cast <int> ( p2.x * worldToScreenScale );
					int lineEndY   = static_cast <int> ( p2.y * worldToScreenScale );

					// Render the trail line segment with the configured thickness and the interpolated fade color.

					renderer->drawLine ( lineStartX, lineStartY, lineEndX, lineEndY, color, trail.thickness );
				}
			}
		}

		// Pass 3: Shadows.

		for ( auto entity : entities )
		{
			// Fetch the shadow, transform, circle, and projection components needed to render the drop shadow.

			auto& shadow     = world.getComponent <ComponentShadow>       ( entity );
			auto& transform  = world.getComponent <ComponentTransform>    ( entity );
			auto& circle     = world.getComponent <ComponentCircle>       ( entity );
			auto& projection = world.getComponent <ComponentProjection2D> ( entity );

			// Compute the per-entity world-to-screen scale using the projection's depth factor.

			double z                  = projection.scale.x;
			double worldToScreenScale = screenHeight * z;

			// Load the shadow texture from the cached resource path; skip this entity if the texture is unavailable.

			SDL_Texture* shadowTexture = renderer->loadTexture ( shadow.imagePath );
			if ( !shadowTexture ) continue;

			// Compute the shadow's screen-space diameter, position (offset from the particle), and draw the shadow texture.

			double diameter        = circle.radius * 2.0 * shadow.scale * worldToScreenScale;
			int    shadowPositionX = static_cast <int> ( ( transform.translation.x + shadow.offset.x ) * worldToScreenScale - diameter / 2.0 );
			int    shadowPositionY = static_cast <int> ( ( transform.translation.y + shadow.offset.y ) * worldToScreenScale - diameter / 2.0 );
			int    shadowWidth     = static_cast <int> ( diameter );
			int    shadowHeight    = static_cast <int> ( diameter );

			renderer->drawTexture ( shadowTexture, shadowPositionX, shadowPositionY, shadowWidth, shadowHeight, shadow.opacity );
		}

		// Pass 4: Sprites.

		for ( auto entity : entities )
		{
			// Fetch the sprite, transform, circle, and projection components needed to render the particle sprite.

			auto& sprite     = world.getComponent <ComponentSprite>       ( entity );
			auto& transform  = world.getComponent <ComponentTransform>    ( entity );
			auto& circle     = world.getComponent <ComponentCircle>       ( entity );
			auto& projection = world.getComponent <ComponentProjection2D> ( entity );

			// Compute the per-entity world-to-screen scale from the projection depth factor.

			double z                  = projection.scale.x;
			double worldToScreenScale = screenHeight * z;

			// Load the sprite texture from the cached resource path; skip this entity if the texture is unavailable.

			SDL_Texture* spriteTexture = renderer->loadTexture ( sprite.imagePath );
			if ( !spriteTexture ) continue;

			// Compute the sprite's screen-space bounding box from its circle radius and draw it centered on the particle.

			double diameter      = circle.radius * 2.0 * worldToScreenScale;
			int    drawPositionX = static_cast <int> ( transform.translation.x * worldToScreenScale - diameter / 2.0 );
			int    drawPositionY = static_cast <int> ( transform.translation.y * worldToScreenScale - diameter / 2.0 );
			int    drawWidth     = static_cast <int> ( diameter );
			int    drawHeight    = static_cast <int> ( diameter );

			renderer->drawTexture ( spriteTexture, drawPositionX, drawPositionY, drawWidth, drawHeight, sprite.opacity );
		}

		// Pass 5: Wireframe circles.

		for ( auto entity : entities )
		{
			// Fetch the circle geometry, transform position, and projection scale for wireframe rendering.

			auto& circle     = world.getComponent <ComponentCircle>       ( entity );
			auto& transform  = world.getComponent <ComponentTransform>    ( entity );
			auto& projection = world.getComponent <ComponentProjection2D> ( entity );

			// Skip particles whose wireframe circle overlay has been toggled off.

			if ( !circle.visible ) continue;

			// Convert the circle's world-space center and radius to screen-space pixel coordinates.

			double z                  = projection.scale.x;
			double worldToScreenScale = screenHeight * z;

			int circlePositionX = static_cast <int> ( transform.translation.x * worldToScreenScale );
			int circlePositionY = static_cast <int> ( transform.translation.y * worldToScreenScale );
			int circleRadius    = static_cast <int> ( circle.radius * worldToScreenScale );

			// Build the wireframe circle color from the circle component's RGB values at full opacity.

			engine::Color color =
			{
				static_cast <uint8_t> ( circle.colorR ),
				static_cast <uint8_t> ( circle.colorG ),
				static_cast <uint8_t> ( circle.colorB ),
				255
			};

			// Render the wireframe circle outline at the computed screen position and radius.

			renderer->drawCircle ( circlePositionX, circlePositionY, circleRadius, color );
		}

		// Pass 6: HUD overlay.

		if ( hudEntity != ecs::NULL_ENTITY && world.hasComponent <ComponentHud> ( hudEntity ) )
		{
			// Retrieve the HUD component from the dedicated HUD entity to access text, position, color, and font size.

			auto& hud = world.getComponent <ComponentHud> ( hudEntity );

			// Only render the HUD if it is marked visible and has non-empty text content to display.

			if ( hud.visible && !hud.text.empty () )
			{
				// Load the HUD font at the configured size; rendering is skipped if the font cannot be loaded.

				TTF_Font* font = renderer->loadFont ( hudFontPath, hud.fontSize );

				// Proceed with HUD rendering only if the font loaded successfully.

				if ( font )
				{
					// Build HUD text from selected particle.

					std::string hudText = buildHudText ( world );

					if ( !hudText.empty () )
					{
						// Construct the HUD text color from the component's RGB values at full opacity.

						engine::Color hudColor =
						{
							static_cast <uint8_t> ( hud.colorR ),
							static_cast <uint8_t> ( hud.colorG ),
							static_cast <uint8_t> ( hud.colorB ),
							255
						};

						// Query the font's line spacing and initialize the vertical cursor at the HUD's configured Y position.

						int lineHeight = TTF_FontLineSkip ( font );
						int y          = static_cast <int> ( hud.position.y );

						// Split the HUD text into individual lines and render each one, advancing the Y cursor by the line height.

						std::istringstream stream ( hudText );
						std::string line;

						// Render each line of HUD text at the configured X position and increment Y for the next line.

						while ( std::getline ( stream, line ) )
						{
							renderer->drawText ( line, static_cast <int> ( hud.position.x ), y, font, hudColor );
							y += lineHeight;
						}
					}
				}
			}
		}

		// Paused indicator.

		if ( worldComponent.paused )
		{
			// Load the pause indicator font at a large size for the centered on-screen "PAUSED" label.

			TTF_Font* pauseFont = renderer->loadFont ( pauseFontPath, 48 );

			// Proceed with pause indicator rendering only if the font loaded successfully.

			if ( pauseFont )
			{
				// Measure the pixel width and height of the "PAUSED" string so it can be horizontally centered on screen.

				int textW = 0;
				int textH = 0;
				TTF_SizeUTF8 ( pauseFont, "PAUSED", &textW, &textH );

				// Center the text horizontally and position it near the bottom of the screen.

				int x = ( screenWidth - textW ) / 2;
				int y = screenHeight - 100;

				// Draw the "PAUSED" label in semi-transparent white at 50% opacity so it does not obscure the scene.

				engine::Color pauseColor = { 255, 255, 255, 128 };
				renderer->drawText ( "PAUSED", x, y, pauseFont, pauseColor, 0.5 );
			}
		}
	}

private:

	//-----------------------------------------------------------------------------------------------------------------
	// Method: buildHudText
	//
	// Description:
	//
	//   Build a formatted multi-line string containing diagnostic information for the currently selected
	//   particle, including entity ID, group, mass, radius, position, velocity, and speed.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	// Returns:
	//
	//   A formatted multi-line string with particle diagnostics, or an empty string if no particle is selected.
	//
	//-----------------------------------------------------------------------------------------------------------------

	std::string buildHudText ( ecs::World& world ) const
	{
		// Find the particle with ComponentUserControl.

		ecs::Entity selected = ecs::NULL_ENTITY;

		for ( auto entity : entities )
		{
			if ( world.hasComponent <ComponentUserControl> ( entity ) )
			{
				selected = entity;
				break;
			}
		}

		// Return an empty string if no particle entity has the user control component attached.

		if ( selected == ecs::NULL_ENTITY ) return "";

		// Retrieve the transform, physics, circle, and particle group components from the selected particle entity.

		auto& transform = world.getComponent <ComponentTransform>    ( selected );
		auto& physics   = world.getComponent <ComponentPhysics>      ( selected );
		auto& circle    = world.getComponent <ComponentCircle>       ( selected );
		auto& group     = world.getComponent <ComponentParticleGroup> ( selected );

		// Compute the scalar speed as the magnitude of the velocity vector for display in the HUD.

		double speed = physics.velocity.length ();

		// Format the particle diagnostics as a multi-line string with fixed-precision decimal values.

		std::ostringstream infoStream;
		infoStream << std::fixed << std::setprecision ( 4 );
		infoStream << "Particle: " << selected << "\n";
		infoStream << "Group:    " << group.groupEntity << "\n";
		infoStream << "Mass:     " << physics.mass << "\n";
		infoStream << "Radius:   " << circle.radius << "\n";
		infoStream << "Position: (" << transform.translation.x << ", " << transform.translation.y << ")\n";
		infoStream << "Velocity: (" << physics.velocity.x << ", " << physics.velocity.y << ")\n";
		infoStream << "Speed:    " << speed << "\n";

		// Return the fully assembled HUD diagnostic string for rendering by the caller.

		return infoStream.str ();
	}
};
