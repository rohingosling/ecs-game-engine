//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentProjection2D struct, an ECS component that stores 2D window-to-viewport projection
//   parameters for mapping simulation coordinates to screen space.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"

//*********************************************************************************************************************
// Struct: ComponentProjection2D
//
// Description:
//
//   An ECS component that stores the window and viewport coordinate bounds, origin, scale, aspect ratio, and depth
//   layer for a 2D projection transform.
//
//   The SystemRenderer uses these parameters to convert simulation-space positions to screen-space pixel coordinates.
//
//*********************************************************************************************************************

struct ComponentProjection2D
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::Vector2D origin     = { 1.0,  1.0 };
	engine::Vector2D scale      = { 1.0, -1.0 };
	engine::Vector2D aspect     = { 1.0,  1.0 };
	double           windowX1   = 0.0;
	double           windowY1   = 0.0;
	double           windowX2   = 0.0;
	double           windowY2   = 0.0;
	double           viewPortX1 = 0.0;
	double           viewPortY1 = 0.0;
	double           viewPortX2 = 0.0;
	double           viewPortY2 = 0.0;
	double           layer      = 0.0;
};
