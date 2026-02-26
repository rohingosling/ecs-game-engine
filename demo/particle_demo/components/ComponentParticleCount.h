//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentParticleCount struct, an ECS component that stores a particle count value with configurable
//   minimum and maximum bounds for the settings menu.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <string>

//*********************************************************************************************************************
// Struct: ComponentParticleCount
//
// Description:
//
//   An ECS component that stores a mutable particle count value along with its allowed range and a display label.
//
//   Attached to settings menu buttons to allow the user to adjust per-group particle counts.
//
//*********************************************************************************************************************

struct ComponentParticleCount
{
	//=================================================================================================================
	// Data members
	//=================================================================================================================

	std::string label            = "Particle Count";
	int         particleCount    = 3;
	int         particleCountMin = 0;
	int         particleCountMax = 32;

};
