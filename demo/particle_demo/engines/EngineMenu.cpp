//---------------------------------------------------------------------------------------------------------------------
// Project: ECS Game Engine - Particle Simulator
// Version: 1.0
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Compilation unit for the EngineMenu class.
//
//   Implements construction, frame presentation, input processing, keyboard command routing, and ECS world
//   initialization with menu entities.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#include "EngineMenu.h"

#include "../components/ComponentButtonState.h"
#include "../components/ComponentButtonImage.h"
#include "../components/ComponentButtonText.h"
#include "../components/ComponentParticleCount.h"
#include "../components/ComponentRectangle.h"
#include "../components/ComponentBackgroundImage.h"
#include "../components/ComponentTextBox.h"

#include "../systems/SystemMenuManager.h"
#include "../systems/SystemMenuRenderer.h"

#include <fstream>
#include <sstream>

//=====================================================================================================================
// Constructors
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// Constructor 1/1: EngineMenu
//
// Description:
//
//   Construct the menu engine with references to shared application resources.
//
//   Stores references to settings, global cache, window, renderer, and keyboard, then initializes the ECS world.
//
// Arguments:
//
//   settings (engine::ApplicationSettings&):
//     Reference to the application settings for reading menu configuration.
//
//   globalCache (engine::GlobalCache&):
//     Reference to the global cache for reading and writing application state.
//
//   window (engine::SDLWindow&):
//     Reference to the SDL window for frame presentation.
//
//   sdlRenderer (engine::SDLRenderer&):
//     Reference to the SDL renderer for drawing operations.
//
//   keyboard (engine::SDLKeyboard&):
//     Reference to the SDL keyboard handler for polling input events.
//
//---------------------------------------------------------------------------------------------------------------------

EngineMenu::EngineMenu
(
	engine::ApplicationSettings& settings,
	engine::GlobalCache&         globalCache,
	engine::SDLWindow&           window,
	engine::SDLRenderer&         sdlRenderer,
	engine::SDLKeyboard&         keyboard
)
	: settings    ( settings )
	, globalCache ( globalCache )
	, window      ( window )
	, sdlRenderer ( sdlRenderer )
	, keyboard    ( keyboard )
{
	resourcePath = settings.getString ( "Application.Resource.Path" );
	initialize ();
}

//=====================================================================================================================
// Methods
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// Method: swapBuffer
//
// Description:
//
//   Present the current frame to the window and process keyboard input.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineMenu::swapBuffer ()
{
	// Flip the back buffer to the screen, displaying the frame rendered by the systems.

	window.present ();

	// Poll keyboard events and route any key presses to the menu manager system.

	processInput ();
}

//---------------------------------------------------------------------------------------------------------------------
// Method: processInput
//
// Description:
//
//   Poll SDL events and dispatch keyboard commands.
//
//   Posts a deferred stop command if the window is closed.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineMenu::processInput ()
{
	// Poll SDL events. A false return indicates the window was closed (SDL_QUIT received).

	if ( !keyboard.pollEvents () )
	{
		// Defer the engine stop to the next command flush to avoid shutting down mid-frame.

		commandManager.post ( [ this ] () { stop (); } );

		// Signal the application state machine that we are shutting down.

		globalCache.put ( "applicationState", 2 );  // STOPPING

		// Skip keyboard command processing since the application is shutting down.

		return;
	}

	// Translate any key presses into menu actions, then mark the frame's input as consumed.

	handleKeyboardCommands ();
	keyboard.endFrame ();
}

//---------------------------------------------------------------------------------------------------------------------
// Method: handleKeyboardCommands
//
// Description:
//
//   Map keyboard key presses to SystemMenuManager action codes for navigation, selection, and increment/decrement
//   operations.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineMenu::handleKeyboardCommands ()
{
	auto systemMenuManager = world.getSystem <SystemMenuManager> ( "MenuManager" );

	if ( !systemMenuManager ) return;

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_UP ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_SELECT_PREV;
	}

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_DOWN ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_SELECT_NEXT;
	}

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_RIGHT ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_INCREMENT;
	}

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_LEFT ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_DECREMENT;
	}

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_RETURN ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_BUTTON_DOWN;
	}

	if ( keyboard.isKeyReleased ( SDL_SCANCODE_RETURN ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_BUTTON_UP;
	}

	if ( keyboard.isKeyPressed ( SDL_SCANCODE_ESCAPE ) )
	{
		systemMenuManager->pendingAction = SystemMenuManager::ACTION_ESC;
	}
}

//---------------------------------------------------------------------------------------------------------------------
// Method: initialize
//
// Description:
//
//   Register all menu component types and systems, create button and text box entities from application settings, and
//   configure system references.
//
//---------------------------------------------------------------------------------------------------------------------

void EngineMenu::initialize ()
{
	// Read screen dimensions for positioning and layout calculations.

	int screenWidth  = settings.getInt ( "Application.Screen.Width" );
	int screenHeight = settings.getInt ( "Application.Screen.Height" );

	// Register all component types used by menu entities. Each call assigns a unique bit position in the signature
	// bitset.
	//
	// Components must be registered before any system that depends on them.

	world.registerComponent <ComponentButtonState>     ();
	world.registerComponent <ComponentButtonImage>     ();
	world.registerComponent <ComponentButtonText>      ();
	world.registerComponent <ComponentParticleCount>   ();
	world.registerComponent <ComponentRectangle>       ();
	world.registerComponent <ComponentBackgroundImage> ();
	world.registerComponent <ComponentTextBox>         ();

	// Register the menu manager system. It matches any entity with a ComponentButtonState and handles menu
	// navigation, screen switching, and action dispatch.

	auto menuManagerSignature = world.makeSignature <ComponentButtonState> ();
	auto systemMenuManager    = world.registerSystem <SystemMenuManager> ( "MenuManager", menuManagerSignature );

	// Register the menu renderer system. It uses the same signature as the manager so it sees the same set of button
	// entities, and draws them in multi-pass order each frame.

	auto menuRendererSignature = world.makeSignature <ComponentButtonState> ();
	auto systemMenuRenderer    = world.registerSystem <SystemMenuRenderer> ( "MenuRenderer", menuRendererSignature );

	// Load button image paths.

	std::string buttonImageUp           = resourcePath + settings.getString ( "Menu.Button.Image.Up"           );
	std::string buttonImageUpSelected   = resourcePath + settings.getString ( "Menu.Button.Image.UpSelected"   );
	std::string buttonImageDown         = resourcePath + settings.getString ( "Menu.Button.Image.Down"         );
	std::string buttonImageDownSelected = resourcePath + settings.getString ( "Menu.Button.Image.DownSelected" );
	std::string buttonImageDisabled     = resourcePath + settings.getString ( "Menu.Button.Image.Disabled"     );
	std::string buttonImageShadow       = resourcePath + settings.getString ( "Menu.Button.Image.Shadow"       );

	// Button layout.

	int buttonPositionX        = settings.getInt ( "Menu.Button.Layout.X"         );
	int buttonPositionYStart   = settings.getInt ( "Menu.Button.Layout.Y.Start"   );
	int buttonPositionYSpacing = settings.getInt ( "Menu.Button.Layout.Y.Spacing" );
	int buttonFontSize         = settings.getInt ( "Menu.Button.Font.Size"        );

	// Create background entity.

	ecs::Entity backgroundEntity = world.createEntity ();

	ComponentBackgroundImage componentBackgroundImage;
	componentBackgroundImage.imagePath = resourcePath + settings.getString ( "Menu.Background.Main" );
	world.addComponent ( backgroundEntity, componentBackgroundImage );

	// Helper lambda to create a menu button.

	auto createButton = [ & ] ( int index, const std::string& text ) -> ecs::Entity
	{
		// Allocate a new entity and attach all the components needed for a standard menu button.

		ecs::Entity buttonEntity = world.createEntity ();

		// Attach a default button state (enabled, not selected, not pressed).

		ComponentButtonState state;
		world.addComponent ( buttonEntity, state );

		// Assign the full set of button sprite paths for each visual state.

		ComponentButtonImage img;

		img.imageUp           = buttonImageUp;
		img.imageUpSelected   = buttonImageUpSelected;
		img.imageDown         = buttonImageDown;
		img.imageDownSelected = buttonImageDownSelected;
		img.imageDisabled     = buttonImageDisabled;
		img.imageShadow       = buttonImageShadow;

		world.addComponent ( buttonEntity, img );

		// Set the label text and font size rendered over the button sprite.

		ComponentButtonText componentButtonText;
		componentButtonText.text = text;
		componentButtonText.size = buttonFontSize;
		world.addComponent ( buttonEntity, componentButtonText );

		// Position the button vertically based on its index within the menu column.

		ComponentRectangle rect;
		rect.origin = { static_cast< double >( buttonPositionX ), static_cast< double >( buttonPositionYStart + index * buttonPositionYSpacing ) };
		world.addComponent ( buttonEntity, rect );

		// Return the fully assembled button entity to the caller.

		return buttonEntity;
	};

	// Create main menu buttons.

	ecs::Entity buttonStart        = createButton ( 0, "Start Simulation" );
	ecs::Entity buttonSettings     = createButton ( 1, "Settings"         );
	ecs::Entity buttonInstructions = createButton ( 2, "Instructions"     );
	ecs::Entity buttonAbout        = createButton ( 3, "About"            );
	ecs::Entity buttonExit         = createButton ( 4, "Exit"             );

	// Create settings counter buttons.

	auto createCounterButton = [ & ] ( int index, const std::string& label, const std::string& countKey, int defaultCount, int minCount, int maxCount ) -> ecs::Entity
	{
		ecs::Entity buttonEntity = createButton ( index, label + ":  < " + std::to_string ( defaultCount ) + " >" );

		// Disable initially (not on main screen).

		world.getComponent <ComponentButtonState> ( buttonEntity ).enabled = false;

		ComponentParticleCount componentParticleCount;

		componentParticleCount.particleCount    = defaultCount;
		componentParticleCount.particleCountMin = minCount;
		componentParticleCount.particleCountMax = maxCount;
		componentParticleCount.label            = label;

		world.addComponent ( buttonEntity, componentParticleCount );

		// Return the counter button with its particle count range configured.

		return buttonEntity;
	};

	// Read current particle counts from the global cache so counter buttons display the values carried over from a
	// previous simulation or the application defaults.

	int redDefault    = globalCache.get <int> ( "particleCountRed"    );
	int greenDefault  = globalCache.get <int> ( "particleCountGreen"  );
	int blueDefault   = globalCache.get <int> ( "particleCountBlue"   );
	int yellowDefault = globalCache.get <int> ( "particleCountYellow" );

	// Create one counter button per particle color group, each with its own min/max range.

	ecs::Entity buttonCountRed    = createCounterButton ( 0, "Red",    "particleCountRed",    redDefault,     settings.getInt ( "Particle.Count.Red.Min" ),    settings.getInt ( "Particle.Count.Red.Max" )    );
	ecs::Entity buttonCountGreen  = createCounterButton ( 1, "Green",  "particleCountGreen",  greenDefault,   settings.getInt ( "Particle.Count.Green.Min" ),  settings.getInt ( "Particle.Count.Green.Max" )  );
	ecs::Entity buttonCountBlue   = createCounterButton ( 2, "Blue",   "particleCountBlue",   blueDefault,    settings.getInt ( "Particle.Count.Blue.Min" ),   settings.getInt ( "Particle.Count.Blue.Max" )   );
	ecs::Entity buttonCountYellow = createCounterButton ( 3, "Yellow", "particleCountYellow", yellowDefault,  settings.getInt ( "Particle.Count.Yellow.Min" ), settings.getInt ( "Particle.Count.Yellow.Max" ) );

	ecs::Entity buttonBack = createButton ( 4, "Back" );
	world.getComponent <ComponentButtonState> ( buttonBack ).enabled = false;

	// Create text box entities.

	auto createTextBox = [ & ] ( const std::string& textFile, int align ) -> ecs::Entity
	{
		// Allocate an entity for the text box and configure it from settings.

		ecs::Entity entity = world.createEntity ();

		// Use ComponentButtonState for enable/disable tracking.

		ComponentButtonState state;
		state.enabled = false;
		world.addComponent ( entity, state );

		// Populate the text box component with layout, font, color, and content from settings.

		ComponentTextBox tb;
		tb.font        = settings.getString ( "Menu.TextBox.Font.Name" );
		tb.fontSize    = settings.getInt    ( "Menu.TextBox.Font.Size" );
		tb.align       = align;
		tb.position.x  = settings.getDouble ( "Menu.TextBox.X" );
		tb.position.y  = settings.getDouble ( "Menu.TextBox.Y" );
		tb.width       = settings.getInt    ( "Menu.TextBox.Width" );
		tb.height      = settings.getInt    ( "Menu.TextBox.Height" );
		tb.textOpacity = settings.getDouble ( "Menu.TextBox.Text.Opacity" );

		// Parse text color.

		int r = 255, g = 255, b = 255;
		sscanf ( settings.getString ( "Menu.TextBox.Font.Color" ).c_str (), "%d,%d,%d", &r, &g, &b );
		tb.colorR = r;
		tb.colorG = g;
		tb.colorB = b;

		// Load text from file.

		std::string filePath = resourcePath + textFile;
		std::ifstream file ( filePath );

		if ( file.is_open () )
		{
			std::ostringstream stringStream;
			stringStream << file.rdbuf ();
			tb.text = stringStream.str ();
		}

		world.addComponent ( entity, tb );

		// Return the fully assembled text box entity.

		return entity;
	};

	// Create the instructions and about text boxes. Both start disabled and are toggled on by the menu manager when
	// their respective screen becomes active.

	ecs::Entity textBoxInstructions = createTextBox ( settings.getString ( "Menu.Text.Instructions" ), 0 );		// left align
	ecs::Entity textBoxAbout        = createTextBox ( settings.getString ( "Menu.Text.About" ),        1 );     // center align

	// Wire up the menu manager with references to all entities, shared state, and configuration so it can
	// orchestrate navigation, screen switching, and actions.

	systemMenuManager->backgroundEntity    = backgroundEntity;
	systemMenuManager->buttonStart         = buttonStart;
	systemMenuManager->buttonSettings      = buttonSettings;
	systemMenuManager->buttonInstructions  = buttonInstructions;
	systemMenuManager->buttonAbout         = buttonAbout;
	systemMenuManager->buttonExit          = buttonExit;
	systemMenuManager->buttonCountRed      = buttonCountRed;
	systemMenuManager->buttonCountGreen    = buttonCountGreen;
	systemMenuManager->buttonCountBlue     = buttonCountBlue;
	systemMenuManager->buttonCountYellow   = buttonCountYellow;
	systemMenuManager->buttonBack          = buttonBack;
	systemMenuManager->textBoxInstructions = textBoxInstructions;
	systemMenuManager->textBoxAbout        = textBoxAbout;
	systemMenuManager->globalCache         = &globalCache;
	systemMenuManager->settings            = &settings;
	systemMenuManager->engineRunning       = &running;

	// Background image paths for screen switching.

	systemMenuManager->backgroundMain         = resourcePath + settings.getString ( "Menu.Background.Main"         );
	systemMenuManager->backgroundSettings     = resourcePath + settings.getString ( "Menu.Background.Settings"     );
	systemMenuManager->backgroundInstructions = resourcePath + settings.getString ( "Menu.Background.Instructions" );
	systemMenuManager->backgroundAbout        = resourcePath + settings.getString ( "Menu.Background.About"        );

	// Selected button color.

	int selectedColorRed = 255, selectedColorGreen = 255, selectedColorBlue = 255;

	sscanf ( settings.getString ( "Menu.Button.Font.Color.Selected" ).c_str (), "%d,%d,%d", &selectedColorRed, &selectedColorGreen, &selectedColorBlue );
	systemMenuManager->selectedColorR = selectedColorRed;
	systemMenuManager->selectedColorG = selectedColorGreen;
	systemMenuManager->selectedColorB = selectedColorBlue;

	// Wire up the menu renderer with the SDL renderer, entity lists, font paths, and visual settings so it can draw
	// buttons, text boxes, and backgrounds each frame.

	systemMenuRenderer->renderer         = &sdlRenderer;
	systemMenuRenderer->settings         = &settings;
	systemMenuRenderer->screenWidth      = screenWidth;
	systemMenuRenderer->screenHeight     = screenHeight;
	systemMenuRenderer->backgroundEntity = backgroundEntity;
	systemMenuRenderer->buttonEntities   = { buttonStart, buttonSettings, buttonInstructions, buttonAbout, buttonExit, buttonCountRed, buttonCountGreen, buttonCountBlue, buttonCountYellow, buttonBack };
	systemMenuRenderer->textBoxEntities  = { textBoxInstructions, textBoxAbout };
	systemMenuRenderer->buttonFontPath   = resourcePath + "Fonts/AnitaSemiSquare.ttf";
	systemMenuRenderer->textBoxFontPath  = resourcePath + "Fonts/cour.ttf";

	// Shadow settings.

	systemMenuRenderer->shadowOffsetX  = settings.getInt    ( "Menu.Button.Shadow.Offset.X" );
	systemMenuRenderer->shadowOffsetY  = settings.getInt    ( "Menu.Button.Shadow.Offset.Y" );
	systemMenuRenderer->shadowOpacity  = settings.getDouble ( "Menu.Button.Shadow.Opacity"  );
	systemMenuRenderer->selectedColorR = selectedColorRed;
	systemMenuRenderer->selectedColorG = selectedColorGreen;
	systemMenuRenderer->selectedColorB = selectedColorBlue;
}
