//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Hello World
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the TerminalSystem class, an ECS system that prints text messages to the standard output console.
//
//   Each matching entity's message is printed exactly once, controlled by the MessageStatusComponent flag.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../ecs/System.h"
#include "../../../ecs/World.h"
#include "../components/TextComponent.h"
#include "../components/MessageStatusComponent.h"

#include <iostream>

//*********************************************************************************************************************
// Class: TerminalSystem
//
// Description:
//
//   An ECS system that iterates over entities possessing both a TextComponent and a MessageStatusComponent.
//
//   On the first frame each entity is encountered, the system prints the entity's text to standard output and sets 
//   the printed flag to prevent duplicate output on subsequent frames.
//
//*********************************************************************************************************************

class TerminalSystem : public ecs::System
{
public:

    //=================================================================================================================
    // Methods
    //=================================================================================================================

    //-----------------------------------------------------------------------------------------------------------------
    // Method: update
    //
    // Description:
    //
    //   Iterate over all matched entities and print each entity's text to the terminal if it has not already been
    //   printed.
    //
    //   Sets the MessageStatusComponent's printed flag to true after output.
    //
    // Arguments:
    //
    //   world (ecs::World&):
    //     Reference to the ECS World, providing access to entity components.
    //
    //   dt (double):
    //     Delta time in seconds since the previous frame. Unused by this system.
    //
    //-----------------------------------------------------------------------------------------------------------------

    void update(ecs::World& world, double dt) override
    {
        // Iterate over all entities matched by this system's component signature.

        for (auto entity : entities)
        {
            // Retrieve the text and message status components for the current entity.

            auto& text   = world.getComponent<TextComponent>(entity);
            auto& status = world.getComponent<MessageStatusComponent>(entity);

            // Print the message once, then set the printed flag to suppress future output.

            if (!status.printed)
            {
                std::cout << "\n\n" << text.text << "\n";
                std::cout << "\nPress any key to exit..." << std::endl;
                status.printed = true;
            }
        }
    }
};
