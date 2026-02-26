//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentBackgroundImage struct, an ECS component that stores background image rendering properties
//   including file path, scale, and position.
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
// Struct: ComponentBackgroundImage
//
// Description:
//
//   An ECS component that stores the file path, scale factor, and screen position for a background image.
//
//   Used by renderer systems to draw full-screen or positioned background textures.
//
//*********************************************************************************************************************

struct ComponentBackgroundImage
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string  imagePath;

	double           scale    = 1.0;
	engine::Vector2D position = { 0.0, 0.0 };
};
