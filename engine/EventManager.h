//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Event struct and EventManager class, implementing a publish-subscribe event system with type-erased
//   payloads.
//
//   Events are queued and dispatched to registered listeners during a flush cycle.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <any>
#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

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
	// Struct: Event
	//
	// Description:
	//
	//   Lightweight event structure carrying a named event identifier and an optional type-erased payload.
	//
	//   Used as the message unit passed between publishers and subscribers through the EventManager.
	//
	//*****************************************************************************************************************

	struct Event
	{
		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::string name;
		std::any    payload;
	};

	using EventListener = std::function <void ( const Event& )>;

	//*****************************************************************************************************************
	// Class: EventManager
	//
	// Description:
	//
	//   Implements a publish-subscribe event system with deferred dispatch.
	//
	//   Listeners register for named events via subscribe, events are enqueued with post, and all queued events are 
	//   dispatched to their registered listeners in FIFO order when flush is called.
	//
	//*****************************************************************************************************************

	class EventManager
	{
	private:

		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		std::queue <Event>                                              eventQueue;
		std::unordered_map <std::string, std::vector <EventListener>>   listeners;

	public:

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: subscribe
		//
		// Description:
		//
		//   Register a listener callback for a named event.
		//
		//   The listener will be invoked whenever an event with the matching name is dispatched during a flush cycle.
		//
		// Arguments:
		//
		//   eventName (const std::string&):
		//     The name of the event to subscribe to.
		//
		//   listener (EventListener):
		//     The callback function to invoke when the named event is dispatched.
		//
		//-------------------------------------------------------------------------------------------------------------

		void subscribe ( const std::string& eventName, EventListener listener )
		{
			listeners [ eventName ].push_back ( std::move ( listener ) );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: post
		//
		// Description:
		//
		//   Enqueue an event with the given name and optional payload for deferred dispatch. 
		//
		//   The event will be delivered to all matching listeners when flush is called.
		//
		// Arguments:
		//
		//   eventName (const std::string&):
		//     The name identifying the event type.
		//
		//   payload (std::any):
		//     An optional type-erased payload to attach to the event.
		//
		//-------------------------------------------------------------------------------------------------------------

		void post ( const std::string& eventName, std::any payload = {} )
		{
			eventQueue.push ( { eventName, std::move ( payload ) } );
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: flush
		//
		// Description:
		//
		//   Dispatch all queued events in FIFO order, invoking every registered listener for each event. 
		//
		//   The queue is drained completely before returning.
		//
		//-------------------------------------------------------------------------------------------------------------

		void flush ()
		{
			// Process every queued event in FIFO order, draining the queue completely before returning.

			while ( !eventQueue.empty () )
			{
				// Dequeue the next event by moving it out of the front of the queue, then pop the now-empty front element.

				Event event = std::move ( eventQueue.front () );
				eventQueue.pop ();

				// Look up all listeners registered for this event name. If any subscribers exist, dispatch the event to them.

				auto it = listeners.find ( event.name );
				if ( it != listeners.end () )
				{
					// Invoke each registered listener callback, passing the current event by const reference.

					for ( auto& listener : it->second )
					{
						listener ( event );
					}
				}
			}
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: clear
		//
		// Description:
		//
		//   Discard all queued events and remove all registered listeners, resetting the manager to an empty state.
		//
		//-------------------------------------------------------------------------------------------------------------

		void clear ()
		{
			std::queue <Event> empty;
			eventQueue.swap ( empty );
			listeners.clear ();
		}
	};
}
