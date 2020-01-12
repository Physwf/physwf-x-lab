#include "Modules/ModuleManager.h"

FModuleManager::~FModuleManager()
{

}

FModuleManager& FModuleManager::Get()
{
	static FModuleManager* ModuleManager = nullptr;

	if (ModuleManager == nullptr)
	{
		ModuleManager = new FModuleManager();
	}
	return ModuleManager;
}

IModuleInterface* FModuleManager::GetModule(const char* InModuleName)
{

}

bool FModuleManager::IsModuleLoaded(const char* InModuleName) const
{

}

IModuleInterface* FModuleManager::LoadModule(const char* InModuleName)
{

}

IModuleInterface& FModuleManager::LoadModuleChecked(const char* InModuleName)
{

}

IModuleInterface* FModuleManager::LoadModuleWithFailureReason(const char* InModuleName)
{

}

FModuleManager::FModuleManager()
{

}

