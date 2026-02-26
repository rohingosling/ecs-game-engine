//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentSprite struct, an ECS component that stores sprite rendering properties including image
//   path, scale, and opacity.
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
// Struct: ComponentSprite
//
// Description:
//
//   An ECS component that stores the texture file path, scale vector, and opacity for rendering a sprite.
//
//   The SystemRenderer uses these properties to draw particle sprites at the correct size and transparency.
//
//*********************************************************************************************************************

struct ComponentSprite
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string      imagePath;
	engine::Vector2D scale   = { 1.0, 1.0 };
	double           opacity = 1.0;
};
