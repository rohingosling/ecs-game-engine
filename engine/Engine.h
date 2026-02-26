//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the abstract Engine base class, which provides a fixed-timestep main loop integrating the ECS World,
//   CommandManager, and ResourceManager.
//
//   Derived classes override swapBuffer for platform-specific rendering.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../ecs/World.h"
#include "CommandManager.h"
#include "ResourceManager.h"

#include <chrono>
#include <thread>

//---------------------------------------------------------------------------------------------------------------------
// Namespace: engine
//
// Description:
//
//   Core namespace for the game engine framework. 
//
//   Contains math utilities, platform abstractions, resource management, and application infrastructure used to build 
//   game applications on top of the ECS layer.
//
//---------------------------------------------------------------------------------------------------------------------

namespace engine
{
	//*****************************************************************************************************************
	// Class: Engine
	//
	// Description:
	//
	//   Abstract base class for the game engine, providing a fixed-timestep main loop that integrates the ECS World,
	//   CommandManager, and ResourceManager. 
	//
	//   Manages frame rate regulation and exposes lifecycle hooks for derived engines to override platform-specific
	//   rendering via swapBuffer.
	//
	//*****************************************************************************************************************

	class Engine
	{
	protected:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		ecs::World       world;
		CommandManager   commandManager;
		ResourceManager  resourceManager;

		bool   running          = false;
		bool   fpsTargetEnabled = true;
		double targetFPS        = 90.0;
		int    fixedDelayMs     = 1000;
		int    minDelayMs       = 5;
		double dt               = 0.0;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: isRunning
		//
		// Description:
		//
		//   Check whether the engine's main loop is currently executing.
		//
		// Returns:
		//
		//   True if the main loop is running, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool isRunning () const
		{
			return running;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getWorld
		//
		// Description:
		//
		//   Return a mutable reference to the internal ECS World instance.
		//
		// Returns:
		//
		//   A mutable reference to the engine's ECS World.
		//
		//-------------------------------------------------------------------------------------------------------------

		ecs::World& getWorld ()
		{
			return world;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getCommandManager
		//
		// Description:
		//
		//   Return a mutable reference to the internal CommandManager instance.
		//
		// Returns:
		//
		//   A mutable reference to the engine's CommandManager.
		//
		//-------------------------------------------------------------------------------------------------------------

		CommandManager& getCommandManager ()
		{
			return commandManager;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getResourceManager
		//
		// Description:
		//
		//   Return a mutable reference to the internal ResourceManager instance.
		//
		// Returns:
		//
		//   A mutable reference to the engine's ResourceManager.
		//
		//-------------------------------------------------------------------------------------------------------------

		ResourceManager& getResourceManager ()
		{
			return resourceManager;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getTargetFPS
		//
		// Description:
		//
		//   Return the currently configured target frames per second.
		//
		// Returns:
		//
		//   The target frames per second value.
		//
		//-------------------------------------------------------------------------------------------------------------

		double getTargetFPS () const
		{
			return targetFPS;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: getDt
		//
		// Description:
		//
		//   Return the delta time in seconds computed during the most recent frame.
		//
		// Returns:
		//
		//   The delta time in seconds since the previous frame.
		//
		//-------------------------------------------------------------------------------------------------------------

		double getDt () const
		{
			return dt;
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Mutator: setTargetFPS
		//
		// Description:
		//
		//   Set the target frame rate and enable FPS-based frame regulation.
		//
		// Arguments:
		//
		//   fps (double):
		//     The desired frames per second.
		//
		//-------------------------------------------------------------------------------------------------------------

		void setTargetFPS ( double fps )
		{
			targetFPS = fps;
			fpsTargetEnabled = true;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Mutator: setFixedDelay
		//
		// Description:
		//
		//   Set a fixed delay per frame in milliseconds and disable FPS-based frame regulation.
		//
		// Arguments:
		//
		//   ms (int):
		//     The fixed delay in milliseconds between frames.
		//
		//-------------------------------------------------------------------------------------------------------------

		void setFixedDelay ( int ms )
		{
			fixedDelayMs = ms;
			fpsTargetEnabled = false;
		}

		//=============================================================================================================
		// Destructor
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Destructor: ~Engine
		//
		// Description:
		//
		//   Virtual destructor ensuring proper cleanup of derived engine types when destroyed through a base pointer.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual ~Engine () = default;

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: swapBuffer
		//
		// Description:
		//
		//   Virtual hook invoked at the end of each frame to present the rendered output.
		//
		//   Default implementation is a no-op; derived engines override this for platform-specific buffer swapping.
		//
		//-------------------------------------------------------------------------------------------------------------

		virtual void swapBuffer () {}

		//-------------------------------------------------------------------------------------------------------------
		// Method: run
		//
		// Description:
		//
		//   Enter the main game loop.
		//
		//   Each iteration flushes deferred commands, updates all ECS systems, swaps the render buffer, regulates the
		//   frame rate, and computes the delta time for the next frame.
		//
		//-------------------------------------------------------------------------------------------------------------

		void run ()
		{
			running = true;

			auto previous = std::chrono::high_resolution_clock::now ();

			while ( running )
			{
				auto frameStart = std::chrono::high_resolution_clock::now ();

				// Flush deferred commands.

				commandManager.flush ();

				// Update all registered systems.

				world.updateSystems ( dt );

				// Swap the render buffer (overridden by graphical engines).

				swapBuffer ();

				// Regulate frame rate.

				regulateFrameRate ( frameStart );

				// Compute dt for the next frame.

				auto frameEnd = std::chrono::high_resolution_clock::now ();
				dt            = std::chrono::duration <double> ( frameEnd - frameStart ).count ();
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: stop
		//
		// Description:
		//
		//   Signal the main loop to exit after the current frame completes by setting the running flag to false.
		//
		//-------------------------------------------------------------------------------------------------------------

		void stop ()
		{
			running = false;
		}

	private:

		//-------------------------------------------------------------------------------------------------------------
		// Method: regulateFrameRate
		//
		// Description:
		//
		//   Sleep the current thread to maintain the configured frame rate.
		//
		//   Computes the remaining time in the frame budget and sleeps for that duration if positive.
		//
		// Arguments:
		//
		//   frameStart (std::chrono::high_resolution_clock::time_point):
		//     The time point recorded at the beginning of the current frame.
		//
		//-------------------------------------------------------------------------------------------------------------

		void regulateFrameRate ( std::chrono::high_resolution_clock::time_point frameStart )
		{
			// Desired milliseconds per frame, computed from the target FPS or the fixed delay setting.

			int targetDelayMs;

			// Choose the frame budget: derive it from the target FPS when FPS regulation is active, otherwise use the
			// fixed delay value directly.

			if ( fpsTargetEnabled )
			{
				targetDelayMs = static_cast< int >( 1000.0 / targetFPS );
			}
			else
			{
				targetDelayMs = fixedDelayMs;
			}

			// Clamp the target delay to the minimum threshold to prevent excessively short or zero-length sleep intervals.

			if ( targetDelayMs < minDelayMs )
			{
				targetDelayMs = minDelayMs;
			}

			// Measure how many milliseconds have already been consumed by this frame's update, render, and swap work.

			auto elapsed = std::chrono::high_resolution_clock::now () - frameStart;

			int  elapsedMs = static_cast< int >
			(
				std::chrono::duration_cast< std::chrono::milliseconds > ( elapsed ).count ()
			);

			// Sleep for the remaining frame budget. If the frame took longer than the target, skip sleeping entirely.

			int sleepMs = targetDelayMs - elapsedMs;

			if ( sleepMs > 0 )
			{
				std::this_thread::sleep_for ( std::chrono::milliseconds ( sleepMs ) );
			}
		}
	};
}
