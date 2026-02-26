//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentTextBox struct, an ECS component that stores multi-line text box rendering properties for
//   menu content screens.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../engine/math/Vector2D.h"

#include <string>

//*********************************************************************************************************************
// Struct: ComponentTextBox
//
// Description:
//
//   An ECS component that stores multi-line text content, font properties, text alignment, color, opacity, position,
//   and bounding dimensions for a scrollable text region.
//
//   Used by the SystemMenuRenderer to display instructions and about screens.
//
//*********************************************************************************************************************

struct ComponentTextBox
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string      text;
	std::string      font        = "Courier New";
	engine::Vector2D position    = { 0.0, 0.0 };
	int              fontSize    = 20;
	int              align       = 1;       // 0=left, 1=center, 2=right
	int              colorR      = 255;
	int              colorG      = 255;
	int              colorB      = 255;
	double           textOpacity = 1.0;
	int              width       = 800;
	int              height      = 600;
};
