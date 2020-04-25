#pragma once

#include "CoreTypes.h"
#include "HAL/Event.h"

#include "X0.h"

/**
* This class is allows a simple one-shot scoped event.
*
* Usage:
* {
*		FScopedEvent MyEvent;
*		SendReferenceOrPointerToSomeOtherThread(&MyEvent); // Other thread calls MyEvent->Trigger();
*		// MyEvent destructor is here, we wait here.
* }
*/
class FScopedEvent
{
public:

	/** Default constructor. */
	X0_API FScopedEvent();

	/** Destructor. */
	X0_API ~FScopedEvent();

	/** Triggers the event. */
	void Trigger()
	{
		Event->Trigger();
	}

	/**
	* Checks if the event has been triggered (used for special early out cases of scope event)
	* if this returns true once it will return true forever
	*
	* @return returns true if the scoped event has been triggered once
	*/
	X0_API bool IsReady();

	/**
	* Retrieve the event, usually for passing around.
	*
	* @return The event.
	*/
	FEvent* Get()
	{
		return Event;
	}

private:

	/** Holds the event. */
	FEvent * Event;
};
