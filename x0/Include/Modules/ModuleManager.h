#pragma once

#include "ModuleInterface.h"

class X0_API XModuleManager
{
public:

	~XModuleManager();

	static XModuleManager& Get();

public:

	void AddModule();
};



