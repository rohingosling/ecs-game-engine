//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Hello World
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the MessageStatusComponent struct, an ECS component that tracks whether a message entity has already
//   been printed to the terminal.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

//*********************************************************************************************************************
// Struct: MessageStatusComponent
//
// Description:
//
//   An ECS component that acts as a one-shot flag indicating whether the associated message has been output.
//
//   The TerminalSystem checks this flag to ensure each message is printed exactly once.
//
//*********************************************************************************************************************

struct MessageStatusComponent
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	bool printed = false;
};
