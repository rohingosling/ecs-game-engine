//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemParticleGroupPropagator class, an ECS system that copies group-level configuration
//   properties down to individual member particles each frame.
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
#include "../components/ComponentSprite.h"
#include "../components/ComponentShadow.h"
#include "../components/ComponentCircle.h"
#include "../components/ComponentPhysics.h"
#include "../components/ComponentTrail.h"
#include "../components/ComponentProjection2D.h"
#include "../components/ComponentUserControl.h"

//*********************************************************************************************************************
// Class: SystemParticleGroupPropagator
//
// Description:
//
//   An ECS system that propagates shared configuration from group template entities to their member
//   particles.
//
//   Copies sprite, shadow, circle, physics, trail, and projection properties while preserving
//   per-particle state such as velocity, position, and user-control overrides.
//
//*********************************************************************************************************************

class SystemParticleGroupPropagator : public ecs::System
{
public:

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Iterate over all matched particle entities and copy configuration properties from each particle's
	//   group template entity.
	//
	//   Sprite and trail color are skipped for user-controlled particles to preserve their selected
	//   appearance.
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
		// Iterate over all particle entities that belong to a group and propagate shared properties from the group template.

		for ( auto entity : entities )
		{
			// Retrieve the particle's group membership component and verify the group template entity is still alive.

			auto& group = world.getComponent <ComponentParticleGroup> ( entity );

			if ( !world.isAlive ( group.groupEntity ) ) continue;

			bool hasUserControl = world.hasComponent <ComponentUserControl> ( entity );

			// Note:
			// - We use bare block scope to group related copy operations and allow reuse of variable names
			//   like "sprite" and "shadow" across different component types without conflicts.

			// Copy sprite (only if not user-controlled).

			if ( !hasUserControl )
			{
				auto& groupSprite = world.getComponent <ComponentSprite> ( group.groupEntity );
				auto& sprite      = world.getComponent <ComponentSprite> ( entity );
				sprite.imagePath  = groupSprite.imagePath;
			}

			// Copy shadow.

			{
				auto& groupShadow = world.getComponent <ComponentShadow> ( group.groupEntity );
				auto& shadow      = world.getComponent <ComponentShadow> ( entity );
				shadow.imagePath  = groupShadow.imagePath;
				shadow.offset     = groupShadow.offset;
				shadow.opacity    = groupShadow.opacity;
				shadow.scale      = groupShadow.scale;
			}

			// Copy circle geometry (radius, visible - keep origin and color).

			{
				auto& groupCircle = world.getComponent <ComponentCircle> ( group.groupEntity );
				auto& circle      = world.getComponent <ComponentCircle> ( entity );
				circle.radius     = groupCircle.radius;
				circle.visible    = groupCircle.visible;
			}

			// Copy physics (mass, friction, elasticity - NOT velocity or force).

			{
				auto& groupPhysics            = world.getComponent <ComponentPhysics> ( group.groupEntity );
				auto& physics                 = world.getComponent <ComponentPhysics> ( entity );
				physics.mass                  = groupPhysics.mass;
				physics.frictionCoefficient   = groupPhysics.frictionCoefficient;
				physics.elasticityCoefficient = groupPhysics.elasticityCoefficient;
			}

			// Copy trail (color only if not user-controlled; depth/opacity/thickness always).

			{
				auto& groupTrail = world.getComponent <ComponentTrail> ( group.groupEntity );
				auto& trail      = world.getComponent <ComponentTrail> ( entity );

				// Propagate trail color only to non-user-controlled particles; user particles keep their distinct trail color.

				if ( !hasUserControl )
				{
					trail.colorR = groupTrail.colorR;
					trail.colorG = groupTrail.colorG;
					trail.colorB = groupTrail.colorB;
				}

				// Always propagate trail depth, opacity, and thickness from the group template regardless of user control.

				trail.depth       = groupTrail.depth;
				trail.opacityHead = groupTrail.opacityHead;
				trail.opacityTail = groupTrail.opacityTail;
				trail.thickness   = groupTrail.thickness;
			}

			// Copy projection scale.

			{
				auto& groupProjection = world.getComponent <ComponentProjection2D> ( group.groupEntity );
				auto& projection      = world.getComponent <ComponentProjection2D> ( entity );
				projection.scale      = groupProjection.scale;
			}
		}
	}
};
