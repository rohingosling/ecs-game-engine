//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemGravity class, an ECS system that applies pairwise Newtonian gravitational attraction
//   between all particles using a softened inverse-square force law.
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
#include "../components/ComponentCircle.h"
#include "../components/ComponentWorld.h"

#include <cmath>
#include <vector>

//*********************************************************************************************************************
// Class: SystemGravity
//
// Description:
//
//   An ECS system that computes pairwise gravitational attraction between all particle entities.
//
//   - Uses a softened inverse-square law with a configurable epsilon to prevent singularities at close
//     range.
//
//   - Overlapping particles are skipped to avoid double-counting with the collision system.
//
//*********************************************************************************************************************

class SystemGravity : public ecs::System
{
public:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	ecs::Entity worldEntity      = ecs::NULL_ENTITY;
	double      softeningEpsilon = 0.009;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Compute pairwise gravitational forces between all particle entities and accumulate them into each
	//   particle's force accumulator.
	//
	//   Skips pairs that are overlapping or closer than their combined radii.
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
		// Early out if no world entity has been assigned to this system.

		if ( worldEntity == ecs::NULL_ENTITY ) return;

		auto& worldComponent = world.getComponent <ComponentWorld> ( worldEntity );

		// Skip gravity computation if the simulation is paused or gravity is disabled.

		if ( worldComponent.paused || !worldComponent.gravityEnabled ) return;

		double gravitationalConstant = worldComponent.gravitationalConstant;

		// Collect all particle entities into a vector for double-loop access.

		std::vector <ecs::Entity> particles ( entities.begin (), entities.end () );
		std::size_t n = particles.size ();

		for ( std::size_t i = 0; i < n; ++i )
		{
			// Fetch transform, physics, and circle components for particle A.

			auto& transformEntityA = world.getComponent <ComponentTransform> ( particles [ i ] );
			auto& physicsEntityA   = world.getComponent <ComponentPhysics>   ( particles [ i ] );
			auto& circleEntityA    = world.getComponent <ComponentCircle>    ( particles [ i ] );

			for ( std::size_t j = i + 1; j < n; ++j )
			{
				// Fetch transform, physics, and circle components for particle B.

				auto& transformEntityB = world.getComponent <ComponentTransform> ( particles [ j ] );
				auto& physicsEntityB   = world.getComponent <ComponentPhysics>   ( particles [ j ] );
				auto& circleEntityB    = world.getComponent <ComponentCircle>    ( particles [ j ] );

				// Compute the displacement vector, squared distance, and Euclidean distance between particle centers.

				double deltaX          = transformEntityB.translation.x - transformEntityA.translation.x;
				double deltaY          = transformEntityB.translation.y - transformEntityA.translation.y;
				double distanceSquared = deltaX * deltaX + deltaY * deltaY;
				double distance        = std::sqrt ( distanceSquared );

				// Skip gravity if particles are overlapping (collider handles contact).

				if ( distance <= circleEntityA.radius + circleEntityB.radius ) continue;

				double distanceSquaredSoftened = distanceSquared + softeningEpsilon * softeningEpsilon;
				double forceMagnitude          = gravitationalConstant * physicsEntityA.mass * physicsEntityB.mass / distanceSquaredSoftened;

				// Compute the unit normal direction and project the gravitational force onto each axis, then accumulate.

				double normalX = deltaX / distance;
				double normalY = deltaY / distance;
				double forceX  = forceMagnitude * normalX;
				double forceY  = forceMagnitude * normalY;

				physicsEntityA.forceAccumulator.x += forceX;
				physicsEntityA.forceAccumulator.y += forceY;
				physicsEntityB.forceAccumulator.x -= forceX;
				physicsEntityB.forceAccumulator.y -= forceY;
			}
		}
	}
};
