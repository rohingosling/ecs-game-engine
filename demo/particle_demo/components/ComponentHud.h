//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentHud struct, an ECS component that stores heads-up display text rendering properties for an
//   overlay information panel.
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
// Struct: ComponentHud
//
// Description:
//
//   An ECS component that stores the visibility flag, text content, font properties, foreground and background
//   colors, and screen position for a heads-up display overlay.
//
//   The SystemRenderer uses these properties to draw diagnostic text for the selected particle.
//
//*********************************************************************************************************************

struct ComponentHud
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string  text;
	std::string  font        = "Courier New";
	engine::Vector2D position    = { 10.0, 10.0 };
	bool         visible     = false;
	int          fontSize    = 14;
	int          colorR      = 0;
	int          colorG      = 255;
	int          colorB      = 0;
	int          backgroundR = 0;
	int          backgroundG = 0;
	int          backgroundB = 0;

};
