//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentCircle struct, an ECS component that stores circle geometry and wireframe display
//   properties for a particle entity.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"

//*********************************************************************************************************************
// Struct: ComponentCircle
//
// Description:
//
//   An ECS component that stores the radius, origin, color, and visibility flags for a wireframe circle overlay.
//
//   Used by the SystemCollider for collision detection and by the SystemRenderer for optional wireframe
//   visualization.
//
//*********************************************************************************************************************

struct ComponentCircle
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	double           radius    = 1.0;
	engine::Vector2D origin    = { 0.0, 0.0 };
	int              colorR    = 255;
	int              colorG    = 255;
	int              colorB    = 255;
	bool             crosshair = true;
	bool             visible   = false;
};
