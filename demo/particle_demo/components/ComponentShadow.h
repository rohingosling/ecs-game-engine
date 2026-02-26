//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentShadow struct, an ECS component that stores drop shadow rendering properties for a sprite
//   entity.
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
// Struct: ComponentShadow
//
// Description:
//
//   An ECS component that stores the shadow texture path, positional offset, opacity, and scale factor.
//
//   The SystemRenderer draws the shadow beneath each particle sprite using these properties.
//
//*********************************************************************************************************************

struct ComponentShadow
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string      imagePath;
	engine::Vector2D offset  = { 10.0, 10.0 };
	double           opacity = 0.8;
	double           scale   = 1.0;
};
