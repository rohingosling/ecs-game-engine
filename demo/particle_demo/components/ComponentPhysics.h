//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentPhysics struct, an ECS component that stores physics simulation state including mass,
//   velocity, accumulated forces, and material coefficients.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"

//*********************************************************************************************************************
// Struct: ComponentPhysics
//
// Description:
//
//   An ECS component that stores the physical properties of a particle including mass, velocity vector, force
//   accumulator, elasticity coefficient, and friction coefficient.
//
//   Updated by the physics pipeline systems each frame.
//
//*********************************************************************************************************************

struct ComponentPhysics
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::Vector2D velocity              = { 0.0, 0.0 };
	engine::Vector2D forceAccumulator      = { 0.0, 0.0 };
	double           mass                  = 1.0;
	double           elasticityCoefficient = 0.9;
	double           frictionCoefficient   = 0.995;
};
