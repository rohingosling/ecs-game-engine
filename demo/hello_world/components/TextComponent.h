//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Hello World
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the TextComponent struct, a simple ECS component that stores a text string for display by a rendering
//   or output system.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <string>

//*********************************************************************************************************************
// Struct: TextComponent
//
// Description:
//
//   An ECS component holding a single text string. 
//
//   Attached to entities that need to display or process textual content through a system such as the TerminalSystem.
//
//*********************************************************************************************************************

struct TextComponent
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	std::string text;
};
