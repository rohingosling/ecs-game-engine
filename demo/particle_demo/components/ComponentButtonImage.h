//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentButtonImage struct, an ECS component that stores file paths for each visual state of a UI
//   button (up, down, selected, disabled, shadow).
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <string>

//*********************************************************************************************************************
// Struct: ComponentButtonImage
//
// Description:
//
//   An ECS component that stores the image asset paths for each visual state of a UI button.
//
//   The SystemMenuRenderer selects the appropriate image based on the button's current ComponentButtonState flags.
//
//*********************************************************************************************************************

struct ComponentButtonImage
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string imageUp;
	std::string imageUpSelected;
	std::string imageDown;
	std::string imageDownSelected;
	std::string imageDisabled;
	std::string imageShadow;
};
