//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemPhysics class, an ECS system that performs Euler position integration, friction damping, and
//   trail history recording for particle entities.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../ecs/System.h"
#include "../../../ecs/World.h"
#include "../components/ComponentParticleGroup.h"
#include "../components/ComponentTransform.h"
#include "../components/ComponentPhysics.h"
#include "../components/ComponentTrail.h"
#include "../components/ComponentUserControl.h"
#include "../components/ComponentWorld.h"

//*********************************************************************************************************************
// Class: SystemPhysics
//
// Description:
//
//   An ECS system that integrates particle velocity into position using forward Euler, applies per-frame
//   friction damping with optional anisotropic damping for user-controlled particles, and records each
//   particle's position into its trail history buffer.
//
//*********************************************************************************************************************

class SystemPhysics : public ecs::System
{
public:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	ecs::Entity worldEntity         = ecs::NULL_ENTITY;
	double      anisotropicFriction = 0.99;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Integrate velocity into position, apply friction damping to velocity, and append the current position
	//   to the trail history deque for each particle entity.
	//
	//   Trims trail history to the configured depth limit.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//   dt (double):
	//     Delta time in seconds since the previous frame.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void update ( ecs::World& world, double dt ) override
	{
		// Early-out if the world entity has not been assigned; physics cannot run without global simulation settings.

		if ( worldEntity == ecs::NULL_ENTITY ) return;

		auto& worldComponent = world.getComponent <ComponentWorld> ( worldEntity );

		// Skip all physics processing when the simulation is paused to freeze particle motion.

		if ( worldComponent.paused ) return;

		// Iterate over all matched particle entities to integrate velocity, apply friction, and record trails.

		for ( auto entity : entities )
		{
			// Fetch the transform, physics, and trail components needed for position integration and history tracking.

			auto& transform = world.getComponent <ComponentTransform> ( entity );
			auto& physics   = world.getComponent <ComponentPhysics> ( entity );
			auto& trail     = world.getComponent <ComponentTrail> ( entity );

			// Position integration.

			transform.translation.x += physics.velocity.x * dt;
			transform.translation.y += physics.velocity.y * dt;

			// Friction damping.

			if ( worldComponent.frictionEnabled )
			{
				// User-controlled particles use anisotropic friction: stronger damping on axes without active input.

				if ( world.hasComponent <ComponentUserControl> ( entity ) )
				{
					auto& userControl = world.getComponent <ComponentUserControl> ( entity );

					bool userInputX = userControl.accelerateLeft || userControl.accelerateRight;
					bool userInputY = userControl.accelerateUp   || userControl.accelerateDown;

					double frictionX = userInputX ? physics.frictionCoefficient : anisotropicFriction;
					double frictionY = userInputY ? physics.frictionCoefficient : anisotropicFriction;

					physics.velocity.x *= frictionX;
					physics.velocity.y *= frictionY;
				}
				else
				{
					// Non-user-controlled particles apply the standard isotropic friction coefficient uniformly to both axes.

					physics.velocity.x *= physics.frictionCoefficient;
					physics.velocity.y *= physics.frictionCoefficient;
				}
			}

			// Record trail history.

			trail.history.push_back ( { transform.translation.x, transform.translation.y } );

			while ( static_cast <int> ( trail.history.size () ) > trail.depth )
			{
				trail.history.pop_front ();
			}
		}
	}
};
