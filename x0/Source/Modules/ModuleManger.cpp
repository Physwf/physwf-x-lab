#include "Modules/ModuleManager.h"

XModuleManager::~XModuleManager()
{

}

XModuleManager& XModuleManager::Get()
{
	static XModuleManager Instance;
	return Instance;
}

void XModuleManager::AddModule()
{

}
