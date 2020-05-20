#pragma once

#include "CoreTypes.h"
#include "Misc/OutputDevice.h"

// Null output device.
class X0_API FOutputDeviceNull : public FOutputDevice
{
public:
	/**
	* NULL implementation of Serialize.
	*
	* @param	Data	unused
	* @param	Event	unused
	*/
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FString& Category) override
	{

	}
};