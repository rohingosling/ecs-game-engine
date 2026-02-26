//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemCollider class, an ECS system that resolves wall boundary collisions and iterative
//   pairwise particle-particle elastic collisions.
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
// Class: SystemCollider
//
// Description:
//
//   An ECS system that handles collision detection and response for the particle simulation. 
//
//   Resolves wall boundary reflections with configurable elasticity and performs iterative pairwise overlap 
//   separation with impulse-based velocity exchange for particle-particle collisions.
//
//*********************************************************************************************************************

class SystemCollider : public ecs::System
{
public:

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	ecs::Entity worldEntity         = ecs::NULL_ENTITY;
	int         collisionIterations = 4;
	int         screenWidth         = 1920;
	int         screenHeight        = 1080;

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Resolve wall boundary collisions for all particles, then perform multiple iterations of pairwise 
	//   particle-particle overlap separation and impulse-based elastic collision response.
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

		// Skip collision processing while the simulation is paused.

		if ( worldComponent.paused ) return;

		// World bounds in simulation coordinates.

		double worldWidth  = static_cast< double > ( screenWidth ) / static_cast< double > ( screenHeight );
		double worldHeight = 1.0;

		std::vector <ecs::Entity> particles ( entities.begin (), entities.end () );
		std::size_t n = particles.size ();

		// Wall collisions.

		for ( std::size_t i = 0; i < n; ++i )
		{
			auto& transform = world.getComponent <ComponentTransform> ( particles [ i ] );
			auto& physics   = world.getComponent <ComponentPhysics> ( particles [ i ] );
			auto& circle    = world.getComponent <ComponentCircle> ( particles [ i ] );

			double& positionX  = transform.translation.x;
			double& positionY  = transform.translation.y;
			double& velocityX  = physics.velocity.x;
			double& velocityY  = physics.velocity.y;
			double  radius     = circle.radius;
			double  elasticity = worldComponent.elasticityEnabled ? physics.elasticityCoefficient : 1.0;

			// Left wall.

			if ( positionX - radius < 0.0 )
			{
				positionX = radius;
				velocityX = std::abs ( velocityX ) * elasticity;
			}

			// Right wall.

			if ( positionX + radius > worldWidth )
			{
				positionX = worldWidth - radius;
				velocityX = -std::abs ( velocityX ) * elasticity;
			}

			// Top wall.

			if ( positionY - radius < 0.0 )
			{
				positionY = radius;
				velocityY = std::abs ( velocityY ) * elasticity;
			}

			// Bottom wall.

			if ( positionY + radius > worldHeight )
			{
				positionY = worldHeight - radius;
				velocityY = -std::abs ( velocityY ) * elasticity;
			}
		}

		// Iterative pairwise particle-particle collision detection and response.

		for ( int iter = 0; iter < collisionIterations; ++iter )
		{
			// Iterate over all particles as the primary collision candidate.

			for ( std::size_t i = 0; i < n; ++i )
			{
				// Fetch transform, physics, and circle components for particle A.

				auto& transformA = world.getComponent <ComponentTransform> ( particles [ i ] );
				auto& physicsA   = world.getComponent <ComponentPhysics>   ( particles [ i ] );
				auto& circleA    = world.getComponent <ComponentCircle>    ( particles [ i ] );

				// Test particle A against every subsequent particle to avoid redundant pair checks.

				for ( std::size_t j = i + 1; j < n; ++j )
				{
					// Fetch transform, physics, and circle components for particle B.

					auto& transformB = world.getComponent <ComponentTransform> ( particles [ j ] );
					auto& physicsB   = world.getComponent <ComponentPhysics>   ( particles [ j ] );
					auto& circleB    = world.getComponent <ComponentCircle>    ( particles [ j ] );

					// Compute the displacement vector and Euclidean distance between particle centers.

					double deltaX   = transformB.translation.x - transformA.translation.x;
					double deltaY   = transformB.translation.y - transformA.translation.y;
					double distance = std::sqrt ( deltaX * deltaX + deltaY * deltaY );

					double minimumDistance = circleA.radius + circleB.radius;

					// Skip this pair if the particles are not overlapping or are coincident (zero distance).

					if ( distance >= minimumDistance || distance <= 0.0 ) continue;

					// Normal from A to B.

					double normalX = deltaX / distance;
					double normalY = deltaY / distance;

					// Positional separation (mass-weighted).

					double overlap     = minimumDistance - distance;
					double totalMass   = physicsA.mass + physicsB.mass;
					double separationA = overlap * ( physicsB.mass / totalMass );
					double separationB = overlap * ( physicsA.mass / totalMass );

					transformA.translation.x -= normalX * separationA;
					transformA.translation.y -= normalY * separationA;
					transformB.translation.x += normalX * separationB;
					transformB.translation.y += normalY * separationB;

					// Impulse-based velocity response (only on first iteration).

					if ( iter == 0 && worldComponent.elasticityEnabled )
					{
						double relativeVelocityX = physicsB.velocity.x - physicsA.velocity.x;
						double relativeVelocityY = physicsB.velocity.y - physicsA.velocity.y;
						double relativeVelocityNormal = relativeVelocityX * normalX + relativeVelocityY * normalY;

						// Only apply if particles are approaching.

						if ( relativeVelocityNormal < 0.0 )
						{
							double restitution = ( physicsA.elasticityCoefficient + physicsB.elasticityCoefficient ) / 2.0;
							double impulse = -( 1.0 + restitution ) * relativeVelocityNormal / ( 1.0 / physicsA.mass + 1.0 / physicsB.mass );

							physicsA.velocity.x -= ( impulse / physicsA.mass ) * normalX;
							physicsA.velocity.y -= ( impulse / physicsA.mass ) * normalY;
							physicsB.velocity.x += ( impulse / physicsB.mass ) * normalX;
							physicsB.velocity.y += ( impulse / physicsB.mass ) * normalY;
						}
					}
				}
			}
		}
	}
};
