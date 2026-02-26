//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentTransform struct, an ECS component that stores spatial transform properties including
//   origin, scale, rotation, and translation.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"
#include "../../../engine/math/Vector3D.h"

//*********************************************************************************************************************
// Struct: ComponentTransform
//
// Description:
//
//   An ECS component that stores the spatial transform of an entity in simulation space, including origin offset,
//   scale factors, Euler rotation angles, and translation position.
//
//*********************************************************************************************************************

struct ComponentTransform
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::Vector2D origin      = { 0.0, 0.0 };
	engine::Vector2D scale       = { 1.0, 1.0 };
	engine::Vector3D rotation    = { 0.0, 0.0, 0.0 };
	engine::Vector2D translation = { 0.0, 0.0 };
};
