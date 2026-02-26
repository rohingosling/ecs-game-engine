//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemForceAccumulator class, an ECS system that converts accumulated forces into velocity
//   changes and applies user input forces for the selected particle.
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
#include "../components/ComponentPhysics.h"
#include "../components/ComponentUserControl.h"
#include "../components/ComponentWorld.h"

//*********************************************************************************************************************
// Class: SystemForceAccumulator
//
// Description:
//
//   An ECS system that resolves accumulated forces into acceleration using Newton's second law, integrates
//   acceleration into velocity, and clears the force accumulator for the next frame.
//
//   Also injects directional forces from user keyboard input for the selected particle.
//
//*********************************************************************************************************************

class SystemForceAccumulator : public ecs::System
{
public:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	ecs::Entity worldEntity = ecs::NULL_ENTITY;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Apply user input forces if present, convert accumulated forces to acceleration, integrate acceleration
	//   into velocity, and reset the force accumulator for each particle entity.
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
		// If the world entity is not set, or if the world is paused, skip force accumulation.

		if ( worldEntity == ecs::NULL_ENTITY ) return;

		auto& worldComponent = world.getComponent <ComponentWorld> ( worldEntity );

		// If the world is paused, skip force accumulation.

		if ( worldComponent.paused ) return;

		// 

		for ( auto entity : entities )
		{
			auto& physics = world.getComponent <ComponentPhysics> ( entity );

			// Apply user input forces if this particle has a ComponentUserControl.

			if ( world.hasComponent <ComponentUserControl> ( entity ) )
			{
				// Retrieve the user control component for this entity.

				auto& userControl = world.getComponent <ComponentUserControl> ( entity );

				// Compute the net force from user input.

				double forceX = 0.0;
				double forceY = 0.0;

				if ( userControl.accelerateUp )    forceY -= userControl.accelerationMagnitude;
				if ( userControl.accelerateDown )  forceY += userControl.accelerationMagnitude;
				if ( userControl.accelerateLeft )  forceX -= userControl.accelerationMagnitude;
				if ( userControl.accelerateRight ) forceX += userControl.accelerationMagnitude;

				physics.forceAccumulator.x += forceX;
				physics.forceAccumulator.y += forceY;
			}

			// F = ma -> a = F/m.

			double ax = physics.forceAccumulator.x / physics.mass;
			double ay = physics.forceAccumulator.y / physics.mass;

			// Update velocity.

			physics.velocity.x += ax * dt;
			physics.velocity.y += ay * dt;

			// Clear force accumulator for next frame.

			physics.forceAccumulator = { 0.0, 0.0 };
		}
	}
};
