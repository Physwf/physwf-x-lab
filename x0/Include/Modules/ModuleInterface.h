#pragma once

#include "X0.h"

class X0_API IModuleInterface
{
public:

	virtual ~IModuleInterface()
	{
	}

	virtual void StartupModule()
	{
	}

	virtual void ShutdownModule()
	{
	}
};