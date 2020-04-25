#pragma once

#include "CoreTypes.h"

#include "X0.h"
/**
* Interface for ticking runnables when there's only one thread available and
* multithreading is disabled.
*/
class X0_API FSingleThreadRunnable
{
public:

	virtual ~FSingleThreadRunnable() { }

	/* Tick function. */
	virtual void Tick() = 0;
};