#pragma once

#include "CoreTypes.h"
#include "Misc/OutputDevice.h"

#include "X0.h"

// Error device.
class X0_API FOutputDeviceError : public FOutputDevice
{
public:
	virtual void HandleError() = 0;
};