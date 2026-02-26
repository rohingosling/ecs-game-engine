//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentButtonText struct, an ECS component that stores text label content and rendering properties
//   for a UI button.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <string>

//*********************************************************************************************************************
// Struct: ComponentButtonText
//
// Description:
//
//   An ECS component that stores the text label and typographic properties for a UI button, including font name,
//   size, alignment, and separate color values for normal and disabled states.
//
//*********************************************************************************************************************

struct ComponentButtonText
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string text           = "-";
	int         align          = 1;      				// 0=left, 1=center, 2=right
	std::string font           = "Anita Semi-square";
	int         size           = 20;
	int         colorR         = 255;
	int         colorG         = 255;
	int         colorB         = 255;
	int         colorDisabledR = 64;
	int         colorDisabledG = 64;
	int         colorDisabledB = 64;
};
