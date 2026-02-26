//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentRectangle struct, an ECS component that stores rectangle geometry and layout properties
//   for UI elements.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"

//*********************************************************************************************************************
// Struct: ComponentRectangle
//
// Description:
//
//   An ECS component that stores the width, height, origin position, color, and visibility flags for a rectangular
//   region.
//
//   Used by the menu system to define button layout positions.
//
//*********************************************************************************************************************

struct ComponentRectangle
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	engine::Vector2D origin    = { 0.0, 0.0 };
	double           width     = 1.0;
	double           height    = 1.0;
	int              colorR    = 255;
	int              colorG    = 255;
	int              colorB    = 255;
	bool             crosshair = true;
	bool             visible   = false;
};
