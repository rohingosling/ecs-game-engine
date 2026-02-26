//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the abstract System base class for the ECS framework.
//
//   Each system declares a component signature that determines which entities it operates on, and provides a pure
//   virtual update method invoked once per frame.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "Entity.h"
#include "Signature.h"

#include <set>
#include <string>

//---------------------------------------------------------------------------------------------------------------------
// Namespace: ecs
//
// Description:
//
//   Core namespace for the Entity Component System framework.
//
//   Contains all ECS types, managers, and system abstractions used to compose game objects through data-driven
//   entity-component relationships.
//
//---------------------------------------------------------------------------------------------------------------------

namespace ecs
{
	class World;

	//*****************************************************************************************************************
	// Class: System
	//
	// Description:
	//
	//   Abstract base class for all ECS systems.
	//
	//   - A system declares a component signature that specifies which components an entity must possess to be
	//     processed by this system.
	//
	//   - The World automatically maintains the set of matching entities.
	//
	//   - Derived classes implement the pure virtual update method to define per-frame behavior.
	//
	//*****************************************************************************************************************

	class System
	{
	public:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::set <Entity> entities;
		std::string       name;
		Signature         signature;
		bool              enabled  = true;

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Destructor
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Destructor: ~System
		//
		// Description:
		//
		//   Virtual destructor ensuring proper cleanup of derived system types when destroyed through a base pointer.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual ~System () = default;

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: update
		//
		// Description:
		//
		//   Pure virtual update method invoked once per frame by the World.
		//
		//   Derived systems implement this to iterate over their matched entity set and perform system-specific logic
		//   such as physics, rendering, or AI.
		//
		// Arguments:
		//
		//   world (World&):
		//     Reference to the ECS World, providing access to entities and components.
		//
		//   dt (double):
		//     Delta time in seconds since the previous frame.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual void update ( World& world, double dt ) = 0;
	};
}
