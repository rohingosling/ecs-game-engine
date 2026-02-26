//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Signature type alias and associated constants for the ECS framework.
//
//   A signature is a fixed-size bitset where each bit corresponds to a registered component type, enabling fast
//   set-membership tests for entities.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <bitset>
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
	static constexpr std::size_t MAX_COMPONENTS = 64;

	using Signature = std::bitset <MAX_COMPONENTS>;
	using ComponentBit = std::size_t;
}
