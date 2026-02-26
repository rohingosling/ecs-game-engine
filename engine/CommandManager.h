//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the CommandManager class, a deferred command queue that stores callable operations and executes them
//   in FIFO order during a flush cycle.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <functional>
#include <queue>

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
	// Class: CommandManager
	//
	// Description:
	//
	//   Manages a queue of deferred commands represented as std::function <void ()> objects.
	//
	//   Commands are posted during processing and later executed in FIFO order when flush is called, enabling safe 
	//   deferred mutation of engine state during iteration.
	//
	//*****************************************************************************************************************

	class CommandManager
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::queue <std::function <void ()>> commandQueue;		// FIFO queue of deferred void() commands awaiting execution on the next flush cycle.

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Predicate Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Predicate Accessor: empty
		//
		// Description:
		//
		//   Check whether the command queue contains any pending commands.
		//
		// Returns:
		//
		//   True if the command queue has no pending commands, false otherwise.
		//
		//-------------------------------------------------------------------------------------------------------------

		bool empty () const
		{
			return commandQueue.empty ();
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: post
		//
		// Description:
		//
		//   Enqueue a callable command for deferred execution.
		//
		//   The command is moved into the internal queue and will be executed when flush is called.
		//
		// Arguments:
		//
		//    (std::function <void ()>):
		//     A callable object or lambda with signature void() representing the deferred command to enqueue.
		//
		//-------------------------------------------------------------------------------------------------------------

		void post ( std::function <void ()> command )
		{
			// Move the command into the back of the queue for deferred execution during the next flush cycle.

			commandQueue.push ( std::move ( command ) );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: flush
		//
		// Description:
		//
		//   Execute and remove all queued commands in FIFO order.
		//
		//   Each command is moved out of the queue and invoked sequentially until the queue is empty.
		//
		//-------------------------------------------------------------------------------------------------------------

		void flush ()
		{
			// Drain the queue front-to-back, moving each command out before invoking it so the queue slot is freed.

			while ( !commandQueue.empty () )
			{
				auto cmd = std::move ( commandQueue.front () );
				commandQueue.pop ();
				cmd ();
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: clear
		//
		// Description:
		//
		//   Discard all pending commands without executing them by swapping the internal queue with an empty queue.
		//
		//-------------------------------------------------------------------------------------------------------------

		void clear ()
		{
			// Swap with a default-constructed empty queue to discard all pending commands without executing them.

			std::queue <std::function <void ()>> empty;
			commandQueue.swap ( empty );
		}
	};
}
