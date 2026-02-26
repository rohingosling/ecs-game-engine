//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the SystemMenuManager class, an ECS system that processes user input actions, manages menu screen
//   transitions, button selection state, and particle count adjustments for the menu UI.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../../../ecs/System.h"
#include "../../../ecs/World.h"
#include "../../../engine/GlobalCache.h"
#include "../../../engine/ApplicationSettings.h"
#include "../components/ComponentButtonState.h"
#include "../components/ComponentButtonText.h"
#include "../components/ComponentParticleCount.h"
#include "../components/ComponentBackgroundImage.h"

#include <string>
#include <vector>

//*********************************************************************************************************************
// Class: SystemMenuManager
//
// Description:
//
//   An ECS system that serves as the state machine for the menu UI.
//
//   Processes pending user actions (navigation, selection, increment/decrement), manages screen transitions
//   between main menu, settings, instructions, and about screens, and synchronizes particle count changes to
//   the global cache.
//
//*********************************************************************************************************************

class SystemMenuManager : public ecs::System
{
public:

	//=================================================================================================================
	// Constants
	//=================================================================================================================

	// Menu action codes for the pendingAction field.
	// - Represents user input actions that the system will process on the next update cycle.

	static constexpr int ACTION_NONE         = 0;
	static constexpr int ACTION_SELECT_NEXT  = 1;
	static constexpr int ACTION_SELECT_PREV  = 2;
	static constexpr int ACTION_ENTER        = 3;
	static constexpr int ACTION_BACK         = 4;
	static constexpr int ACTION_INCREMENT    = 5;
	static constexpr int ACTION_DECREMENT    = 6;
	static constexpr int ACTION_ESC          = 7;
	static constexpr int ACTION_BUTTON_DOWN  = 8;
	static constexpr int ACTION_BUTTON_UP    = 9;
	static constexpr int ACTION_EXIT_APP     = 10;

	// Screen identifiers for the currentScreen field.
	// - Represents the different menu screens that can be active, which determine which buttons are visible and interactable.

	static constexpr int SCREEN_MAIN         = 0;
	static constexpr int SCREEN_SETTINGS     = 1;
	static constexpr int SCREEN_INSTRUCTIONS = 2;
	static constexpr int SCREEN_ABOUT        = 3;

	//=================================================================================================================
	// Data Members
	//=================================================================================================================

	int pendingAction = ACTION_NONE;
	int currentScreen = SCREEN_MAIN;

	ecs::Entity backgroundEntity = ecs::NULL_ENTITY;

	ecs::Entity buttonStart        = ecs::NULL_ENTITY;
	ecs::Entity buttonSettings     = ecs::NULL_ENTITY;
	ecs::Entity buttonInstructions = ecs::NULL_ENTITY;
	ecs::Entity buttonAbout        = ecs::NULL_ENTITY;
	ecs::Entity buttonExit         = ecs::NULL_ENTITY;

	ecs::Entity buttonCountRed    = ecs::NULL_ENTITY;
	ecs::Entity buttonCountGreen  = ecs::NULL_ENTITY;
	ecs::Entity buttonCountBlue   = ecs::NULL_ENTITY;
	ecs::Entity buttonCountYellow = ecs::NULL_ENTITY;
	ecs::Entity buttonBack        = ecs::NULL_ENTITY;

	ecs::Entity textBoxInstructions = ecs::NULL_ENTITY;
	ecs::Entity textBoxAbout        = ecs::NULL_ENTITY;

	engine::GlobalCache* globalCache      = nullptr;
	engine::ApplicationSettings* settings = nullptr;
	bool* engineRunning                   = nullptr;

	std::string backgroundMain;
	std::string backgroundSettings;
	std::string backgroundInstructions;
	std::string backgroundAbout;

	int selectedColorR = 255;
	int selectedColorG = 255;
	int selectedColorB = 255;

	//=================================================================================================================
	// Accessors
	//=================================================================================================================

	//=================================================================================================================
	// Mutators
	//=================================================================================================================

	// None

	//=================================================================================================================
	// Methods
	//=================================================================================================================

	//-----------------------------------------------------------------------------------------------------------------
	// Method: update
	//
	// Description:
	//
	//   Consume the pending action and dispatch to the appropriate handler method based on the action code.
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

	void update ( ecs::World& world, double dt ) override
	{
		// Early out if no user action has been queued since the last frame.

		if ( pendingAction == ACTION_NONE ) return;

		// Consume the pending action by copying it locally and resetting the queue to ACTION_NONE.

		int action    = pendingAction;
		pendingAction = ACTION_NONE;

		switch ( action )
		{
			case ACTION_SELECT_NEXT: selectNext      ( world ); break;
			case ACTION_SELECT_PREV: selectPrevious  ( world ); break;
			case ACTION_ENTER:       enterButton     ( world ); break;
			case ACTION_BACK:        goBack          ( world ); break;
			case ACTION_INCREMENT:   incrementValue  ( world ); break;
			case ACTION_DECREMENT:   decrementValue  ( world ); break;
			case ACTION_ESC:         escAction       ( world ); break;
			case ACTION_BUTTON_DOWN: buttonDown      ( world ); break;
			case ACTION_BUTTON_UP:   buttonUp        ( world ); break;
			case ACTION_EXIT_APP:    exitApplication ();        break;
		}
	}

private:

	//-----------------------------------------------------------------------------------------------------------------
	// Method: getActiveButtonIds
	//
	// Description:
	//
	//   Return the list of button entity IDs that are active on the current menu screen.
	//
	// Returns:
	//
	//   A vector of entity IDs for the buttons visible on the current screen.
	//
	//-----------------------------------------------------------------------------------------------------------------

	std::vector <ecs::Entity> getActiveButtonIds () const
	{
		// Build and return a list of button entity IDs that belong to the currently visible menu screen.

		std::vector <ecs::Entity> ids;

		// Populate the list based on which screen is currently active.

		switch ( currentScreen )
		{
			case SCREEN_MAIN:
				ids = { buttonStart, buttonSettings, buttonInstructions, buttonAbout, buttonExit };
				break;

			case SCREEN_SETTINGS:
				ids = { buttonCountRed, buttonCountGreen, buttonCountBlue, buttonCountYellow, buttonBack };
				break;

			case SCREEN_INSTRUCTIONS:
			case SCREEN_ABOUT:
				ids = { buttonBack };
				break;
		}

		return ids;
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: findSelectedButton
	//
	// Description:
	//
	//   Search all tracked button entities and return the one whose ComponentButtonState has the selected flag set.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	// Returns:
	//
	//   The entity ID of the currently selected button, or NULL_ENTITY if none is selected.
	//
	//-----------------------------------------------------------------------------------------------------------------

	ecs::Entity findSelectedButton ( ecs::World& world ) const
	{
		// Iterate over all entities tracked by this system and return the first one with its selected flag set.

		for ( auto entity : entities )
		{
			// Only examine entities that have a button state component.

			if ( world.hasComponent <ComponentButtonState> ( entity ) )
			{
				// If this button is currently selected, return it immediately.

				auto& state = world.getComponent <ComponentButtonState> ( entity );
				if ( state.selected ) return entity;
			}
		}

		// No button in the entity set has the selected flag set.

		return ecs::NULL_ENTITY;
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: deselectAll
	//
	// Description:
	//
	//   Clear the selected and pressed flags on all tracked button entities.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void deselectAll ( ecs::World& world )
	{
		// Walk every entity in the system's set and clear the selected and pressed flags on all button state components.

		for ( auto entity : entities )
		{
			// Reset selection and press state so no button appears highlighted or depressed.

			if ( world.hasComponent <ComponentButtonState> ( entity ) )
			{
				auto& state    = world.getComponent <ComponentButtonState> ( entity );
				state.selected = false;
				state.pressed  = false;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: selectButton
	//
	// Description:
	//
	//   Deselect all buttons, then set the selected flag on the specified button entity.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//   entity (ecs::Entity):
	//     The button entity to select, or NULL_ENTITY to deselect all.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void selectButton ( ecs::World& world, ecs::Entity entity )
	{
		// Clear the selection from every button first to ensure mutual exclusivity.

		deselectAll ( world );

		// If a valid button entity was specified, mark it as the sole selected button.

		if ( entity != ecs::NULL_ENTITY && world.hasComponent <ComponentButtonState> ( entity ) )
		{
			auto& state    = world.getComponent <ComponentButtonState> ( entity );
			state.selected = true;
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: selectNext
	//
	// Description:
	//
	//   Move the selection to the next button in the active button list for the current screen.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void selectNext ( ecs::World& world )
	{
		// Retrieve the ordered list of button entities visible on the current screen.

		auto ids = getActiveButtonIds ();
		if ( ids.empty () ) return;

		// Determine which button is currently selected so we can advance to the next one.

		ecs::Entity selected = findSelectedButton ( world );

		// If nothing is selected yet, default to selecting the first button in the list.

		if ( selected == ecs::NULL_ENTITY )
		{
			selectButton ( world, ids [ 0 ] );
			return;
		}

		// Walk the button list and select the button immediately after the currently selected one.

		for ( std::size_t i = 0; i < ids.size () - 1; ++i )
		{
			// When we find the currently selected button, move selection forward by one position.

			if ( ids [ i ] == selected )
			{
				selectButton ( world, ids [ i + 1 ] );
				return;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: selectPrevious
	//
	// Description:
	//
	//   Move the selection to the previous button in the active button list for the current screen.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void selectPrevious ( ecs::World& world )
	{
		// Retrieve the ordered list of button entities visible on the current screen.

		auto ids = getActiveButtonIds ();
		if ( ids.empty () ) return;

		ecs::Entity selected = findSelectedButton ( world );

		// If nothing is selected yet, default to selecting the first button in the list.

		if ( selected == ecs::NULL_ENTITY )
		{
			selectButton ( world, ids [ 0 ] );
			return;
		}

		// Walk the button list and select the button immediately before the currently selected one.

		for ( std::size_t i = 1; i < ids.size (); ++i )
		{
			// When we find the currently selected button, move selection backward by one position.

			if ( ids [ i ] == selected )
			{
				selectButton ( world, ids [ i - 1 ] );
				return;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: enterButton
	//
	// Description:
	//
	//   Execute the action associated with the currently selected button, such as starting the simulation, switching
	//   screens, or exiting the application.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void enterButton ( ecs::World& world )
	{
		// Determine which button is currently selected to decide which action to perform.

		ecs::Entity selected = findSelectedButton ( world );

		// If no button is selected, there is nothing to activate.

		if ( selected == ecs::NULL_ENTITY ) return;

		// Dispatch to the appropriate handler based on which button is selected.

		if      ( selected == buttonStart )        startSimulation ( world );
		else if ( selected == buttonSettings )     switchScreen    ( world, SCREEN_SETTINGS );
		else if ( selected == buttonInstructions ) switchScreen    ( world, SCREEN_INSTRUCTIONS );
		else if ( selected == buttonAbout )        switchScreen    ( world, SCREEN_ABOUT );
		else if ( selected == buttonExit )         exitApplication ();
		else if ( selected == buttonBack )         goBack          ( world );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: buttonDown
	//
	// Description:
	//
	//   Set the pressed flag on the currently selected button to trigger the down visual state.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void buttonDown ( ecs::World& world )
	{
		// Find the button that currently has focus so we can apply the pressed visual state.

		ecs::Entity selected = findSelectedButton ( world );

		// If no button is selected, there is nothing to press.

		if ( selected == ecs::NULL_ENTITY ) return;

		// Set the pressed flag so the renderer can draw the button in its depressed state.

		auto& state   = world.getComponent <ComponentButtonState> ( selected );
		state.pressed = true;
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: buttonUp
	//
	// Description:
	//
	//   Clear the pressed flag on the currently selected button and execute its associated action.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void buttonUp ( ecs::World& world )
	{
		// Find the button that currently has focus so we can release its pressed visual state.

		ecs::Entity selected = findSelectedButton ( world );

		// If no button is selected, there is nothing to release.

		if ( selected == ecs::NULL_ENTITY ) return;

		// Clear the pressed flag to restore the button's normal appearance, then activate its action.

		auto& state   = world.getComponent <ComponentButtonState> ( selected );
		state.pressed = false;

		enterButton ( world );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: escAction
	//
	// Description:
	//
	//   Handle the Escape key action.
	//
	//   - On the main screen, select or activate the Exit button.
	//
	//   - On sub-screens, select or activate the Back button.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void escAction ( ecs::World& world )
	{
		// Handle Escape differently depending on whether we are on the main menu or a sub-screen.

		if ( currentScreen == SCREEN_MAIN )
		{
			// On the main screen, pressing Escape selects the Exit button; pressing it again exits the application.

			ecs::Entity selected = findSelectedButton ( world );

			if ( selected == buttonExit )
			{
				exitApplication ();
			}
			else
			{
				selectButton ( world, buttonExit );
			}
		}
		else
		{
			// On a sub-screen, pressing Escape selects the Back button; pressing it again navigates back to main.

			ecs::Entity selected = findSelectedButton ( world );

			if ( selected == buttonBack )
			{
				goBack ( world );
			}
			else
			{
				selectButton ( world, buttonBack );
			}
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: goBack
	//
	// Description:
	//
	//   Navigate back to the main menu screen.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void goBack ( ecs::World& world )
	{
		switchScreen ( world, SCREEN_MAIN );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: switchScreen
	//
	// Description:
	//
	//   Transition the menu to the specified screen by enabling and disabling the appropriate button and text
	//   box entities, updating the background image, and clearing the selection state.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//   screen (int):
	//     The target screen constant (SCREEN_MAIN, SCREEN_SETTINGS, SCREEN_INSTRUCTIONS, or SCREEN_ABOUT).
	//
	//-----------------------------------------------------------------------------------------------------------------

	void switchScreen ( ecs::World& world, int screen )
	{
		// Enable/disable entities per screen.

		auto setEnabled = [ & ] ( ecs::Entity e, bool en )
		{
			if ( e != ecs::NULL_ENTITY && world.hasComponent <ComponentButtonState> ( e ) )
			{
				world.getComponent <ComponentButtonState> ( e ).enabled = en;
			}
		};

		// Enable all first.

		setEnabled ( buttonStart,         true  );
		setEnabled ( buttonSettings,      true  );
		setEnabled ( buttonInstructions,  true  );
		setEnabled ( buttonAbout,         true  );
		setEnabled ( buttonExit,          true  );
		setEnabled ( buttonCountRed,      true  );
		setEnabled ( buttonCountGreen,    true  );
		setEnabled ( buttonCountBlue,     true  );
		setEnabled ( buttonCountYellow,   true  );
		setEnabled ( buttonBack,          true  );
		setEnabled ( textBoxInstructions, false );
		setEnabled ( textBoxAbout,        false );

		// Disable entities not on the target screen.

		switch ( screen )
		{
			case SCREEN_MAIN:
				setEnabled ( buttonCountRed,    false );
				setEnabled ( buttonCountGreen,  false );
				setEnabled ( buttonCountBlue,   false );
				setEnabled ( buttonCountYellow, false );
				setEnabled ( buttonBack,        false );
				break;

			case SCREEN_SETTINGS:
				setEnabled ( buttonStart,        false );
				setEnabled ( buttonSettings,     false );
				setEnabled ( buttonInstructions, false );
				setEnabled ( buttonAbout,        false );
				setEnabled ( buttonExit,         false );
				break;

			case SCREEN_INSTRUCTIONS:
				setEnabled ( buttonStart,         false );
				setEnabled ( buttonSettings,      false );
				setEnabled ( buttonInstructions,  false );
				setEnabled ( buttonAbout,         false );
				setEnabled ( buttonExit,          false );
				setEnabled ( buttonCountRed,      false );
				setEnabled ( buttonCountGreen,    false );
				setEnabled ( buttonCountBlue,     false );
				setEnabled ( buttonCountYellow,   false );
				setEnabled ( textBoxInstructions, true  );
				break;

			case SCREEN_ABOUT:
				setEnabled ( buttonStart,        false );
				setEnabled ( buttonSettings,     false );
				setEnabled ( buttonInstructions, false );
				setEnabled ( buttonAbout,        false );
				setEnabled ( buttonExit,         false );
				setEnabled ( buttonCountRed,     false );
				setEnabled ( buttonCountGreen,   false );
				setEnabled ( buttonCountBlue,    false );
				setEnabled ( buttonCountYellow,  false );
				setEnabled ( textBoxAbout,       true  );
				break;
		}

		// Update background image.

		if ( backgroundEntity != ecs::NULL_ENTITY && world.hasComponent <ComponentBackgroundImage> ( backgroundEntity ) )
		{
			auto& background = world.getComponent <ComponentBackgroundImage> ( backgroundEntity );

			switch ( screen )
			{
				case SCREEN_MAIN:         background.imagePath = backgroundMain;         break;
				case SCREEN_SETTINGS:     background.imagePath = backgroundSettings;     break;
				case SCREEN_INSTRUCTIONS: background.imagePath = backgroundInstructions; break;
				case SCREEN_ABOUT:        background.imagePath = backgroundAbout;        break;
			}
		}

		currentScreen = screen;
		deselectAll ( world );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: incrementValue
	//
	// Description:
	//
	//   Increment the particle count on the currently selected counter button, update its display text, and
	//   synchronize the change to the global cache.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void incrementValue ( ecs::World& world )
	{
		// Find the currently selected button; only counter buttons on the settings screen support increment.

		ecs::Entity selected = findSelectedButton ( world );
		if ( selected == ecs::NULL_ENTITY ) return;

		// Ensure the selected button actually has a particle count component before modifying it.

		if ( !world.hasComponent <ComponentParticleCount> ( selected ) ) return;

		auto& count = world.getComponent <ComponentParticleCount> ( selected );

		// Increase the particle count by one if it has not yet reached the maximum allowed value.

		if ( count.particleCount < count.particleCountMax )
		{
			count.particleCount++;
			updateCounterButtonText ( world, selected, count );
			updateGlobalCache       ( world );
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: decrementValue
	//
	// Description:
	//
	//   Decrement the particle count on the currently selected counter button, update its display text, and
	//   synchronize the change to the global cache.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void decrementValue ( ecs::World& world )
	{
		// Find the currently selected button; only counter buttons on the settings screen support decrement.

		ecs::Entity selected = findSelectedButton ( world );
		if ( selected == ecs::NULL_ENTITY ) return;

		// Ensure the selected button actually has a particle count component before modifying it.

		if ( !world.hasComponent <ComponentParticleCount> ( selected ) ) return;

		auto& count = world.getComponent <ComponentParticleCount> ( selected );

		// Decrease the particle count by one if it has not yet reached the minimum allowed value.

		if ( count.particleCount > count.particleCountMin )
		{
			count.particleCount--;
			updateCounterButtonText ( world, selected, count );
			updateGlobalCache ( world );
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: updateCounterButtonText
	//
	// Description:
	//
	//   Refresh the button's text label to reflect the current particle count value in the format "Label:  < count >".
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//   entity (ecs::Entity):
	//     The counter button entity whose text label should be updated.
	//
	//   count (const ComponentParticleCount&):
	//     The ComponentParticleCount containing the current count and display label.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void updateCounterButtonText ( ecs::World& world, ecs::Entity entity, const ComponentParticleCount& count )
	{
		// Update the button's display text to show the label and current particle count in "Label:  < count >" format.

		if ( world.hasComponent <ComponentButtonText> ( entity ) )
		{
			auto& buttonTextComponent  = world.getComponent <ComponentButtonText> ( entity );
			buttonTextComponent.text   = count.label + ":  < " + std::to_string ( count.particleCount ) + " >";
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: updateGlobalCache
	//
	// Description:
	//
	//   Write the current particle count values from all counter button entities to the global cache.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void updateGlobalCache ( ecs::World& world )
	{
		// Guard against a null global cache pointer, which would occur if the system was not fully initialized.

		if ( !globalCache ) return;

		// Lambda to safely extract the particle count from a counter button entity, returning zero if invalid.

		auto getCount = [ & ] ( ecs::Entity e ) -> int
		{
			if ( e != ecs::NULL_ENTITY && world.hasComponent <ComponentParticleCount> ( e ) )
				return world.getComponent <ComponentParticleCount> ( e ).particleCount;
			return 0;
		};

		// Write each color channel's particle count into the global cache for use by the simulation engine.

		globalCache->put ( "particleCountRed",    getCount ( buttonCountRed )    );
		globalCache->put ( "particleCountGreen",  getCount ( buttonCountGreen )  );
		globalCache->put ( "particleCountBlue",   getCount ( buttonCountBlue )   );
		globalCache->put ( "particleCountYellow", getCount ( buttonCountYellow ) );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: startSimulation
	//
	// Description:
	//
	//   Synchronize particle counts to the global cache, set the application state to particle simulation, and stop
	//   the menu engine.
	//
	// Arguments:
	//
	//   world (ecs::World&):
	//     Reference to the ECS World, providing access to entity components.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void startSimulation ( ecs::World& world )
	{
		// Flush the latest particle count values to the global cache before leaving the menu.

		updateGlobalCache ( world );

		// Signal the application state machine to transition into the particle simulation level.

		if ( globalCache )
		{
			globalCache->put ( "applicationState", 100 );  // APPLICATION_STATE_LEVEL_PARTICLE_SIMULATION
		}

		// Stop the menu engine's run loop so the application can hand control to the simulation engine.

		if ( engineRunning ) *engineRunning = false;
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: exitApplication
	//
	// Description:
	//
	//   Set the application state to stopping and halt the menu engine to terminate the application.
	//
	//-----------------------------------------------------------------------------------------------------------------

	void exitApplication ()
	{
		// Signal the application state machine to begin the shutdown sequence.

		if ( globalCache )
		{
			globalCache->put ( "applicationState", 2 );  // APPLICATION_STATE_STOPPING
		}

		// Stop the menu engine's run loop so the application can proceed with termination.

		if ( engineRunning ) *engineRunning = false;
	}
};
