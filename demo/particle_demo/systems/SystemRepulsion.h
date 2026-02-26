//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemRepulsion class, an ECS system that applies soft short-range repulsive forces between
//   nearby particles to prevent overlap before collision.
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
// Class: SystemRepulsion
//
// Description:
//
//   An ECS system that applies a soft quadratic repulsive force between particle pairs whose separation
//   falls between their combined radii and twice that distance.
//
//   Prevents clustering and provides a smooth transition zone before hard collision response.
//
//*********************************************************************************************************************

class SystemRepulsion : public ecs::System
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
	//   Compute pairwise short-range repulsive forces between all particle entities and accumulate them into
	//   each particle's force accumulator.
	//
	//   Only active when particles are within twice their combined radii but not yet overlapping.
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

		// Retrieve the world configuration component to check simulation state and repulsion parameters.

		auto& worldComponent = world.getComponent <ComponentWorld> ( worldEntity );

		// Skip repulsion computation if the simulation is paused or repulsion is disabled.

		if ( worldComponent.paused || !worldComponent.repulsionEnabled ) return;

		// Cache the repulsive constant and collect all particle entities into a vector for indexed pair iteration.

		double repulsiveConstant = worldComponent.repulsiveConstant;

		std::vector <ecs::Entity> particles ( entities.begin (), entities.end () );
		std::size_t n = particles.size ();

		// Outer loop: iterate over all particles as the primary repulsion candidate.

		for ( std::size_t i = 0; i < n; ++i )
		{
			// Fetch transform, physics, and circle components for particle A.

			auto& transformEntityA = world.getComponent <ComponentTransform> ( particles [ i ] );
			auto& physicsEntityA = world.getComponent <ComponentPhysics> ( particles [ i ] );
			auto& circleEntityA = world.getComponent <ComponentCircle> ( particles [ i ] );

			// Inner loop: test particle A against every subsequent particle to avoid redundant pair checks.

			for ( std::size_t j = i + 1; j < n; ++j )
			{
				// Fetch transform, physics, and circle components for particle B.

				auto& transformEntityB = world.getComponent <ComponentTransform> ( particles [ j ] );
				auto& physicsEntityB   = world.getComponent <ComponentPhysics> ( particles [ j ] );
				auto& circleEntityB    = world.getComponent <ComponentCircle> ( particles [ j ] );

				// Direction from particle 2 to particle 1 (repulsive, pushing away).

				double deltaX   = transformEntityA.translation.x - transformEntityB.translation.x;
				double deltaY   = transformEntityA.translation.y - transformEntityB.translation.y;
				double distance = std::sqrt ( deltaX * deltaX + deltaY * deltaY );

				// Compute the combined radii and the repulsion threshold at twice that distance.

				double minimumDistance = circleEntityA.radius + circleEntityB.radius;
				double threshold       = minimumDistance * 2.0;

				// Skip if particles are overlapping (handled by collider) or beyond the repulsion threshold.

				if ( distance <= minimumDistance || distance >= threshold ) continue;

				// Quadratic falloff: force is strongest at the combined radii boundary and drops to zero at the threshold.

				double scaleFactor         = ( distance - minimumDistance ) / ( threshold - minimumDistance );
				double oneMinusScaleFactor = 1.0 - scaleFactor;
				double forceMagnitude      = repulsiveConstant * oneMinusScaleFactor * oneMinusScaleFactor;

				// Project the repulsive force magnitude along the unit normal direction between the two particles.

				double normalX = deltaX / distance;
				double normalY = deltaY / distance;
				double forceX  = forceMagnitude * normalX;
				double forceY  = forceMagnitude * normalY;

				// Apply equal and opposite repulsive forces to each particle's force accumulator (Newton's third law).

				physicsEntityA.forceAccumulator.x += forceX;
				physicsEntityA.forceAccumulator.y += forceY;
				physicsEntityB.forceAccumulator.x -= forceX;
				physicsEntityB.forceAccumulator.y -= forceY;
			}
		}
	}
};
