//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Compilation unit for the EngineParticleSimulator class.
//
//   Implements construction, frame presentation, input processing, keyboard command handling, and ECS world
//   initialization with particle entities.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "EngineParticleSimulator.h"

#include "../../../engine/math/GMath.h"
#include "../components/ComponentWorld.h"
#include "../components/ComponentBackgroundImage.h"
#include "../components/ComponentParticleGroup.h"
#include "../components/ComponentSprite.h"
#include "../components/ComponentShadow.h"
#include "../components/ComponentCircle.h"
#include "../components/ComponentPhysics.h"
#include "../components/ComponentTransform.h"
#include "../components/ComponentTrail.h"
#include "../components/ComponentProjection2D.h"
#include "../components/ComponentUserControl.h"
#include "../components/ComponentHud.h"

#include "../systems/SystemParticleGroupPropagator.h"
#include "../systems/SystemGravity.h"
#include "../systems/SystemRepulsion.h"
#include "../systems/SystemForceAccumulator.h"
#include "../systems/SystemPhysics.h"
#include "../systems/SystemCollider.h"
#include "../systems/SystemRenderer.h"

#include <string>

//=====================================================================================================================
// Constructors
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------------------------

EngineParticleSimulator::EngineParticleSimulator
(
	engine::ApplicationSettings& settings,
	engine::GlobalCache&         globalCache,
	engine::SDLWindow&           window,
	engine::SDLRenderer&         sdlRenderer,
	engine::SDLKeyboard&         keyboard
)
	: settings    ( settings )
	, globalCache ( globalCache )
	, window      ( window )
	, sdlRenderer ( sdlRenderer )
	, keyboard    ( keyboard )
{
	resourcePath = settings.getString ( "Application.Resource.Path" );
	initialize ();
}

//=====================================================================================================================
// Methods
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// Method: swapBuffer
//
// Description:
//
//   Present the current frame to the window and process keyboard input.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineParticleSimulator::swapBuffer ()
{
	window.present ();
	processInput ();
}

//---------------------------------------------------------------------------------------------------------------------
// Method: processInput
//
// Description:
//
//   Poll SDL events and dispatch keyboard commands.
//
//   Posts a deferred stop command if the window is closed.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineParticleSimulator::processInput ()
{
	// Poll SDL events. If pollEvents returns false, the window close event was received, so post a deferred stop
	// command and set the application state to STOPPING.

	if ( !keyboard.pollEvents () )
	{
		commandManager.post ( [ this ] () { stop (); } );
		globalCache.put ( "applicationState", 2 );  // STOPPING
		return;
	}

	// Dispatch keyboard commands for the current frame, then reset per-frame key state for the next frame.

	handleKeyboardCommands ();
	keyboard.endFrame ();
}

//---------------------------------------------------------------------------------------------------------------------
// Method: handleKeyboardCommands
//
// Description:
//
//   Process keyboard input for simulation controls including Escape (deselect or exit),
//   Tab (cycle particle selection), arrow keys (accelerate selected particle), P (toggle pause), T (toggle trails),
//   and W (toggle wireframe).
//
//---------------------------------------------------------------------------------------------------------------------

void EngineParticleSimulator::handleKeyboardCommands ()
{
	// Escape: if a particle is selected, deselect it and hide the HUD. Otherwise, return to the main menu.

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_ESCAPE ) )
	{
		if ( selectedParticle != ecs::NULL_ENTITY )
		{
			// A particle is currently selected, so post a deferred command to deselect it.

			commandManager.post ( [ this ] ()
			{
				// Remove the user control component from the selected particle and clear the selection.

				if ( selectedParticle != ecs::NULL_ENTITY && world.hasComponent <ComponentUserControl> ( selectedParticle ) )
				{
					world.removeComponent <ComponentUserControl> ( selectedParticle );
				}
				selectedParticle = ecs::NULL_ENTITY;

				// Hide the HUD overlay since no particle is selected.

				if ( hudEntity != ecs::NULL_ENTITY && world.hasComponent <ComponentHud> ( hudEntity ) )
				{
					world.getComponent <ComponentHud> ( hudEntity ).visible = false;
				}
			} );
		}
		else
		{
			// No particle is selected, so exit the simulation and return to the main menu.

			commandManager.post ( [ this ] ()
			{
				globalCache.put ( "applicationState", 10 );  // MENU_MAIN
				stop ();
			} );
		}
	}

	// Tab / Shift+Tab: select next/previous particle.

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_TAB ) )
	{
		bool shiftDown = keyboard.isKeyDown ( SDL_SCANCODE_LSHIFT ) || keyboard.isKeyDown ( SDL_SCANCODE_RSHIFT );

		commandManager.post ( [ this, shiftDown ] ()
		{
			if ( particleEntities.empty () ) return;

			// Remove ComponentUserControl from current selection.

			if ( selectedParticle != ecs::NULL_ENTITY && world.hasComponent <ComponentUserControl> ( selectedParticle ) )
			{
				world.removeComponent <ComponentUserControl> ( selectedParticle );
			}

			// Find next/previous.

			if ( selectedParticle == ecs::NULL_ENTITY )
			{
				selectedParticle = particleEntities [ 0 ];
			}
			else
			{
				// Locate the currently selected particle in the entity list and cycle to the next or previous.

				for ( std::size_t i = 0; i < particleEntities.size (); ++i )
				{
					// Check if this entity matches the current selection.

					if ( particleEntities [ i ] == selectedParticle )
					{
						// Shift+Tab selects the previous particle, Tab selects the next, wrapping around at boundaries.

						if ( shiftDown )
						{
							selectedParticle = ( i > 0 ) ? particleEntities [ i - 1 ] : particleEntities.back ();
						}
						else
						{
							selectedParticle = ( i < particleEntities.size () - 1 ) ? particleEntities [ i + 1 ] : particleEntities [ 0 ];
						}
						break;
					}
				}
			}

			// Add ComponentUserControl to new selection.

			double acceleration = settings.getDouble ( "Physics.User.Acceleration" );
			ComponentUserControl uc;
			uc.accelerationMagnitude = acceleration;
			world.addComponent ( selectedParticle, uc );

			// Update sprite to selected sprite.

			auto& sprite = world.getComponent <ComponentSprite> ( selectedParticle );
			sprite.imagePath = resourcePath + settings.getString ( "Sprite.Selected" );

			// Update trail color to selected trail color.

			auto& trail = world.getComponent <ComponentTrail> ( selectedParticle );
			std::string selTrailColorStr = settings.getString ( "Trail.Color.Selected" );

			// Parse "r,g,b".

			int r = 128, g = 128, b = 128;
			if ( sscanf ( selTrailColorStr.c_str (), "%d,%d,%d", &r, &g, &b ) == 3 )
			{
				trail.colorR = r;
				trail.colorG = g;
				trail.colorB = b;
			}

			// Show HUD.

			if ( hudEntity != ecs::NULL_ENTITY && world.hasComponent <ComponentHud> ( hudEntity ) )
			{
				world.getComponent <ComponentHud> ( hudEntity ).visible = true;
				world.getComponent <ComponentHud> ( hudEntity ).text    = "selected";
			}
		} );
	}

	// Arrow keys: accelerate selected particle.

	if ( selectedParticle != ecs::NULL_ENTITY && world.hasComponent <ComponentUserControl> ( selectedParticle ) )
	{
		auto& uc = world.getComponent <ComponentUserControl> ( selectedParticle );

		uc.accelerateUp    = keyboard.isKeyDown ( SDL_SCANCODE_UP );
		uc.accelerateDown  = keyboard.isKeyDown ( SDL_SCANCODE_DOWN );
		uc.accelerateLeft  = keyboard.isKeyDown ( SDL_SCANCODE_LEFT );
		uc.accelerateRight = keyboard.isKeyDown ( SDL_SCANCODE_RIGHT );
	}

	// P: Toggle pause.

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_P ) )
	{
		commandManager.post ( [ this ] ()
		{
			auto& componentWorld     = world.getComponent <ComponentWorld> ( worldEntity );
			componentWorld.paused    = !componentWorld.paused;
		} );
	}

	// T: Toggle trails.

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_T ) )
	{
		commandManager.post ( [ this ] ()
		{
			auto& componentWorld           = world.getComponent <ComponentWorld> ( worldEntity );
			componentWorld.trailsVisible   = !componentWorld.trailsVisible;
		} );
	}

	// W: Toggle wireframe.

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_W ) )
	{
		commandManager.post
		(
			[ this ] ()
			{
				// Toggle wireframe visibility on all particles.

				for ( auto entity : particleEntities )
				{
					auto& circle    = world.getComponent <ComponentCircle> ( entity );
					circle.visible  = !circle.visible;
				}
			}
		);
	}
}

//---------------------------------------------------------------------------------------------------------------------
// Method: initialize
//
// Description:
//
//   Register all ECS component types and systems, create particle group entities, individual particle entities, and
//   the HUD entity, then configure system references from application settings.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineParticleSimulator::initialize ()
{
	// Read screen dimensions from application settings for coordinate conversions and renderer configuration.

	int screenWidth  = settings.getInt ( "Application.Screen.Width" );
	int screenHeight = settings.getInt ( "Application.Screen.Height" );

	// Register all ECS component types used by the particle simulation with the world.

	world.registerComponent <ComponentWorld>           ();
	world.registerComponent <ComponentBackgroundImage> ();
	world.registerComponent <ComponentParticleGroup>   ();
	world.registerComponent <ComponentSprite>          ();
	world.registerComponent <ComponentShadow>          ();
	world.registerComponent <ComponentCircle>          ();
	world.registerComponent <ComponentPhysics>         ();
	world.registerComponent <ComponentTransform>       ();
	world.registerComponent <ComponentTrail>           ();
	world.registerComponent <ComponentProjection2D>    ();
	world.registerComponent <ComponentUserControl>     ();
	world.registerComponent <ComponentHud>             ();

	// Build the particle component signature and register all simulation systems in execution order. Each system
	// receives the same signature so it operates on entities that have the full set of particle components.

	auto particleSignature             = world.makeSignature <ComponentParticleGroup, ComponentSprite, ComponentShadow, ComponentCircle, ComponentPhysics, ComponentTransform, ComponentTrail, ComponentProjection2D> ();
	auto systemParticleGroupPropagator = world.registerSystem <SystemParticleGroupPropagator> ( "ParticleGroupPropagator", particleSignature );
	auto systemGravity                 = world.registerSystem <SystemGravity>                 ( "Gravity",                 particleSignature );
	auto systemRepulsion               = world.registerSystem <SystemRepulsion>               ( "Repulsion",               particleSignature );
	auto systemForceAccumulator        = world.registerSystem <SystemForceAccumulator>        ( "ForceAccumulator",        particleSignature );
	auto systemPhysics                 = world.registerSystem <SystemPhysics>                 ( "Physics",                 particleSignature );
	auto systemCollider                = world.registerSystem <SystemCollider>                ( "Collider",                particleSignature );
	auto systemRenderer                = world.registerSystem <SystemRenderer>                ( "Renderer",                particleSignature );

	// Create the world entity.

	worldEntity = world.createEntity ();

	ComponentWorld componentWorld;

	componentWorld.particleCountRed      = globalCache.get <int> ( "particleCountRed" );
	componentWorld.particleCountGreen    = globalCache.get <int> ( "particleCountGreen" );
	componentWorld.particleCountBlue     = globalCache.get <int> ( "particleCountBlue" );
	componentWorld.particleCountYellow   = globalCache.get <int> ( "particleCountYellow" );
	componentWorld.gravitationalConstant = settings.getDouble    ( "Physics.Gravity.Constant" );
	componentWorld.repulsiveConstant     = settings.getDouble    ( "Physics.Repulsion.Constant" );
	componentWorld.gravityEnabled        = settings.getBool      ( "Physics.Gravity.Enabled" );
	componentWorld.repulsionEnabled      = settings.getBool      ( "Physics.Repulsion.Enabled" );
	componentWorld.frictionEnabled       = settings.getBool      ( "Physics.Friction.Enabled" );
	componentWorld.elasticityEnabled     = settings.getBool      ( "Physics.Elasticity.Enabled" );
	componentWorld.trailsVisible         = settings.getBool      ( "Trail.Visible" );

	world.addComponent ( worldEntity, componentWorld );

	// Create background image entity.

	ComponentBackgroundImage componentBackgroundImage;

	componentBackgroundImage.imagePath = resourcePath + settings.getString ( "Simulation.Background" );

	world.addComponent ( worldEntity, componentBackgroundImage );

	// Load sprite/shadow paths.

	std::string spriteRed    = resourcePath + settings.getString ( "Sprite.Red" );
	std::string spriteGreen  = resourcePath + settings.getString ( "Sprite.Green" );
	std::string spriteBlue   = resourcePath + settings.getString ( "Sprite.Blue" );
	std::string spriteYellow = resourcePath + settings.getString ( "Sprite.Yellow" );
	std::string spriteShadow = resourcePath + settings.getString ( "Sprite.Shadow" );

	// Define a local configuration struct and color-parsing helper for setting up the four particle groups.

	struct GroupConfig
	{
		std::string sprite;
		int         count;
		double      mass;
		double      radius;
		int         trailR, trailG, trailB;
	};

	auto parseColor = [] ( const std::string& s, int& r, int& g, int& b )
	{
		sscanf ( s.c_str (), "%d,%d,%d", &r, &g, &b );
	};

	// Load physics, trail, projection, wireframe, and shadow parameters from application settings.

	double frictionCoefficient    = settings.getDouble ( "Physics.Friction.Coefficient" );
	double elasticityCoefficient  = settings.getDouble ( "Physics.Elasticity.Coefficient" );
	double projectionZoom         = settings.getDouble ( "Projection.Zoom" );
	int    trailDepth             = settings.getInt    ( "Trail.Depth" );
	double trailOpacityHead       = settings.getDouble ( "Trail.Opacity.Head" );
	double trailOpacityTail       = settings.getDouble ( "Trail.Opacity.Tail" );
	int    trailThickness         = settings.getInt    ( "Trail.Thickness" );
	bool   wireframeVisible       = settings.getBool   ( "Wireframe.Visible" );
	double shadowOffsetX          = settings.getDouble ( "Shadow.Offset.X" );
	double shadowOffsetY          = settings.getDouble ( "Shadow.Offset.Y" );
	double shadowOpacity          = settings.getDouble ( "Shadow.Opacity" );
	double shadowScale            = settings.getDouble ( "Shadow.Scale" );
	double velocityMin            = settings.getDouble ( "Initial.Velocity.Min" );
	double velocityMax            = settings.getDouble ( "Initial.Velocity.Max" );

	// Configure the four particle groups (red, green, blue, yellow) with their sprite, count, mass, radius, and
	// trail color loaded from application settings and the global cache.

	GroupConfig groups [ 4 ];

	groups [ 0 ].sprite = spriteRed;
	groups [ 0 ].count  = componentWorld.particleCountRed;
	groups [ 0 ].mass   = settings.getDouble ( "Particle.Mass.Red" );
	groups [ 0 ].radius = settings.getDouble ( "Particle.Radius.Red" );
	parseColor ( settings.getString ( "Trail.Color.Red" ), groups [ 0 ].trailR, groups [ 0 ].trailG, groups [ 0 ].trailB );

	groups [ 1 ].sprite = spriteGreen;
	groups [ 1 ].count  = componentWorld.particleCountGreen;
	groups [ 1 ].mass   = settings.getDouble ( "Particle.Mass.Green" );
	groups [ 1 ].radius = settings.getDouble ( "Particle.Radius.Green" );
	parseColor ( settings.getString ( "Trail.Color.Green" ), groups [ 1 ].trailR, groups [ 1 ].trailG, groups [ 1 ].trailB );

	groups [ 2 ].sprite  = spriteBlue;
	groups [ 2 ].count   = componentWorld.particleCountBlue;
	groups [ 2 ].mass    = settings.getDouble ( "Particle.Mass.Blue" );
	groups [ 2 ].radius  = settings.getDouble ( "Particle.Radius.Blue" );
	parseColor ( settings.getString ( "Trail.Color.Blue" ), groups [ 2 ].trailR, groups [ 2 ].trailG, groups [ 2 ].trailB );

	groups [ 3 ].sprite = spriteYellow;
	groups [ 3 ].count  = componentWorld.particleCountYellow;
	groups [ 3 ].mass   = settings.getDouble ( "Particle.Mass.Yellow" );
	groups [ 3 ].radius = settings.getDouble ( "Particle.Radius.Yellow" );
	parseColor ( settings.getString ( "Trail.Color.Yellow" ), groups [ 3 ].trailR, groups [ 3 ].trailG, groups [ 3 ].trailB );

	// World bounds for random particle placement.

	double worldWidth  = static_cast < double > ( screenWidth ) / static_cast< double >( screenHeight );
	double worldHeight = 1.0;
	double margin      = 0.05;

	// Create particle group entities and individual particles.

	for ( int g = 0; g < 4; ++g )
	{
		auto& groupConfiguration = groups [ g ];

		// Create group entity (template for propagator).

		ecs::Entity groupEntity = world.createEntity ();

		ComponentSprite groupSprite;
		groupSprite.imagePath = groupConfiguration.sprite;
		world.addComponent ( groupEntity, groupSprite );

		ComponentShadow groupShadow;
		groupShadow.imagePath = spriteShadow;
		groupShadow.offset    = { shadowOffsetX, shadowOffsetY };
		groupShadow.opacity   = shadowOpacity;
		groupShadow.scale     = shadowScale;
		world.addComponent ( groupEntity, groupShadow );

		ComponentCircle groupCircle;
		groupCircle.radius  = groupConfiguration.radius;
		groupCircle.visible = wireframeVisible;
		world.addComponent ( groupEntity, groupCircle );

		ComponentPhysics groupPhysics;
		groupPhysics.mass                   = groupConfiguration.mass;
		groupPhysics.frictionCoefficient    = frictionCoefficient;
		groupPhysics.elasticityCoefficient  = elasticityCoefficient;
		world.addComponent ( groupEntity, groupPhysics );

		ComponentTrail groupTrail;
		groupTrail.colorR      = groupConfiguration.trailR;
		groupTrail.colorG      = groupConfiguration.trailG;
		groupTrail.colorB      = groupConfiguration.trailB;
		groupTrail.depth       = trailDepth;
		groupTrail.opacityHead = trailOpacityHead;
		groupTrail.opacityTail = trailOpacityTail;
		groupTrail.thickness   = trailThickness;
		world.addComponent ( groupEntity, groupTrail );

		ComponentProjection2D groupProjection;
		groupProjection.scale = { projectionZoom, projectionZoom };
		world.addComponent ( groupEntity, groupProjection );

		// Create individual particles.

		for ( int p = 0; p < groupConfiguration.count; ++p )
		{
			ecs::Entity particle = world.createEntity ();

			ComponentParticleGroup pgc;
			pgc.groupEntity = groupEntity;
			world.addComponent ( particle, pgc );

			ComponentSprite sprite;
			sprite.imagePath = groupConfiguration.sprite;
			world.addComponent ( particle, sprite );

			ComponentShadow shadow;
			shadow.imagePath = spriteShadow;
			shadow.offset    = { shadowOffsetX, shadowOffsetY };
			shadow.opacity   = shadowOpacity;
			shadow.scale     = shadowScale;
			world.addComponent ( particle, shadow );

			ComponentCircle circle;
			circle.radius  = groupConfiguration.radius;
			circle.visible = wireframeVisible;
			world.addComponent ( particle, circle );

			ComponentPhysics physics;
			physics.mass                   = groupConfiguration.mass;
			physics.frictionCoefficient    = frictionCoefficient;
			physics.elasticityCoefficient  = elasticityCoefficient;
			physics.velocity.x             = engine::randomInRange ( -velocityMax, velocityMax );
			physics.velocity.y             = engine::randomInRange ( -velocityMax, velocityMax );
			world.addComponent ( particle, physics );

			ComponentTransform transform;
			transform.translation.x = engine::randomInRange ( margin, worldWidth - margin );
			transform.translation.y = engine::randomInRange ( margin, worldHeight - margin );
			world.addComponent ( particle, transform );

			ComponentTrail trail;
			trail.colorR      = groupConfiguration.trailR;
			trail.colorG      = groupConfiguration.trailG;
			trail.colorB      = groupConfiguration.trailB;
			trail.depth       = trailDepth;
			trail.opacityHead = trailOpacityHead;
			trail.opacityTail = trailOpacityTail;
			trail.thickness   = trailThickness;
			world.addComponent ( particle, trail );

			ComponentProjection2D projection;
			projection.scale = { projectionZoom, projectionZoom };
			world.addComponent ( particle, projection );

			particleEntities.push_back ( particle );
		}
	}

	// Create HUD entity.

	hudEntity = world.createEntity ();

	ComponentHud hud;
	hud.visible  = false;
	hud.font     = settings.getString ( "Hud.Font.Name" );
	hud.fontSize = settings.getInt ( "Hud.Font.Size" );

	int hudR = 0, hudG = 255, hudB = 0;
	parseColor ( settings.getString ( "Hud.Font.Color" ), hudR, hudG, hudB );
	hud.colorR = hudR;
	hud.colorG = hudG;
	hud.colorB = hudB;

	hud.position.x = settings.getDouble ( "Hud.Position.X" );
	hud.position.y = settings.getDouble ( "Hud.Position.Y" );
	world.addComponent ( hudEntity, hud );

	// Configure the gravity system with the world entity and a softening epsilon to prevent numerical instability
	// at very short inter-particle distances.

	systemGravity->worldEntity      = worldEntity;
	systemGravity->softeningEpsilon = settings.getDouble ( "Physics.Softening.Epsilon" );

	// Configure the repulsion system with the world entity for reading the repulsive force constant.

	systemRepulsion->worldEntity = worldEntity;

	// Configure the force accumulator system with the world entity for reading the pause state.

	systemForceAccumulator->worldEntity = worldEntity;

	// Configure the physics system with the world entity and the anisotropic friction coefficient for
	// directional drag.

	systemPhysics->worldEntity         = worldEntity;
	systemPhysics->anisotropicFriction = settings.getDouble ( "Physics.Friction.Anisotropic" );

	// Configure the collider system with the world entity, iteration count for iterative collision resolution, and
	// screen dimensions for boundary clamping.

	systemCollider->worldEntity         = worldEntity;
	systemCollider->collisionIterations = settings.getInt ( "Physics.Collision.Iterations" );
	systemCollider->screenWidth         = screenWidth;
	systemCollider->screenHeight        = screenHeight;

	// Configure the renderer system with the SDL renderer, world and HUD entities, screen dimensions, and font paths.

	systemRenderer->renderer      = &sdlRenderer;
	systemRenderer->worldEntity   = worldEntity;
	systemRenderer->hudEntity     = hudEntity;
	systemRenderer->screenWidth   = screenWidth;
	systemRenderer->screenHeight  = screenHeight;
	systemRenderer->hudFontPath   = resourcePath + "Fonts/cour.ttf";
	systemRenderer->pauseFontPath = resourcePath + "Fonts/cour.ttf";
}
