//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentButtonState struct, an ECS component that tracks the interactive state of a UI button,
//   including whether it is pressed, selected, or enabled.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

//*********************************************************************************************************************
// Struct: ComponentButtonState
//
// Description:
//
//   An ECS component that tracks the interactive state of a UI button.
//
//   Used by the menu system to determine visual appearance and input handling for each button entity.
//
//*********************************************************************************************************************

struct ComponentButtonState
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	bool pressed  = false;
	bool selected = false;
	bool enabled  = true;
};
