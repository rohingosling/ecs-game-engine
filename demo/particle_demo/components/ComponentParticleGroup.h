//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentParticleGroup struct, an ECS component that links an individual particle entity to its
//   group template entity.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../ecs/Entity.h"

//*********************************************************************************************************************
// Struct: ComponentParticleGroup
//
// Description:
//
//   An ECS component that associates an individual particle entity with its group template entity.
//
//   The SystemParticleGroupPropagator uses this reference to copy shared configuration from the group down to each
//   member particle.
//
//*********************************************************************************************************************

struct ComponentParticleGroup
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	ecs::Entity groupEntity = ecs::NULL_ENTITY;
};
