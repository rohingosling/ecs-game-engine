//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentUserControl struct, an ECS component that stores directional acceleration flags and
//   magnitude for a keyboard-controlled particle.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

//*********************************************************************************************************************
// Struct: ComponentUserControl
//
// Description:
//
//   An ECS component that stores per-frame directional acceleration flags and the acceleration magnitude for a
//   user-selected particle.
//
//   The SystemForceAccumulator reads these flags to apply keyboard-driven forces, and the SystemPhysics applies
//   anisotropic friction damping.
//
//*********************************************************************************************************************

struct ComponentUserControl
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	bool   accelerateUp          = false;
	bool   accelerateDown        = false;
	bool   accelerateLeft        = false;
	bool   accelerateRight       = false;
	double accelerationMagnitude = 100.0;
};
