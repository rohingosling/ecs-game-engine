//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the ComponentWorld struct, an ECS component that stores global simulation parameters including particle
//   counts, physics constants, and feature toggle flags.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

//*********************************************************************************************************************
// Struct: ComponentWorld
//
// Description:
//
//   An ECS component that stores global simulation configuration on a singleton world entity, including per-group
//   particle counts, gravitational and repulsive force constants, pause state, and boolean toggles for trails,
//   gravity, repulsion, friction, and elasticity.
//
//*********************************************************************************************************************

struct ComponentWorld
{
	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	int    particleCountRed      = 6;
	int    particleCountGreen    = 3;
	int    particleCountBlue     = 3;
	int    particleCountYellow   = 9;
	double gravitationalConstant = 1.0;
	double repulsiveConstant     = 500.0;
	bool   paused                = false;
	bool   trailsVisible         = true;
	bool   gravityEnabled        = true;
	bool   repulsionEnabled      = true;
	bool   frictionEnabled       = true;
	bool   elasticityEnabled     = true;
};
