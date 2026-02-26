//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentTrail struct, an ECS component that stores motion trail rendering history and visual
//   properties for a particle entity.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"

#include <deque>

//*********************************************************************************************************************
// Struct: ComponentTrail
//
// Description:
//
//   An ECS component that maintains a bounded deque of historical positions and stores trail rendering properties
//   including color, opacity gradient, depth limit, and line thickness.
//
//   The SystemPhysics records positions and the SystemRenderer draws the trail.
//
//*********************************************************************************************************************

struct ComponentTrail
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::deque <engine::Vector2D> history;

	int    depth       = 500;
	int    colorR      = 64;
	int    colorG      = 64;
	int    colorB      = 64;
	double opacityTail = 0.0;
	double opacityHead = 0.5;
	int    thickness   = 4;
};
