//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Entity type alias and associated constants for the ECS framework.
//
//   An entity is a lightweight 32-bit unsigned integer identifier with a reserved null value and a configurable
//   upper bound on active entities.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <cstdint>

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
	using Entity = uint32_t;

	static constexpr Entity NULL_ENTITY = 0;
	static constexpr Entity MAX_ENTITIES = 4096;
}
