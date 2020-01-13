#include "Modules/ModuleManager.h"
#include <Windows.h>

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
	return *ModuleManager;
}

void FModuleManager::AddModule(const std::string& InModuleName)
{
	if (Modules.find(InModuleName) != Modules.end()) return;

	std::shared_ptr<FModuleInfo> ModuleInfo(new FModuleInfo());

	std::map<std::string, std::string> ModulePathMap;
	FindModulePaths(InModuleName.c_str(), ModulePathMap);

	std::string ModuleFileName = ModulePathMap.begin()->second;
	ModuleInfo->Filename = ModuleFileName;

	FModuleManager::Get().AddModuleToModulesList(InModuleName, ModuleInfo);
}

IModuleInterface* FModuleManager::GetModule(const std::string& InModuleName)
{
	std::shared_ptr<FModuleInfo> ModuleInfo = FindModule(InModuleName);

	if (!ModuleInfo)
	{
		return nullptr;
	}

	return ModuleInfo->Module.get();
}

bool FModuleManager::IsModuleLoaded(const std::string& InModuleName) const
{
	std::shared_ptr<FModuleInfo> ModuleInfo = FindModule(InModuleName);
	if (ModuleInfo)
	{
		if (ModuleInfo->Module)
		{
			return true;
		}
	}
	return false;
}

IModuleInterface* FModuleManager::LoadModule(const std::string& InModuleName)
{
	IModuleInterface* Result = LoadModuleWithFailureReason(InModuleName);
	return Result;
}

IModuleInterface& FModuleManager::LoadModuleChecked(const std::string& InModuleName)
{
	IModuleInterface* Module = LoadModule(InModuleName);
	return *Module;
}

IModuleInterface* FModuleManager::LoadModuleWithFailureReason(const std::string& InModuleName)
{
	IModuleInterface* LoadedModule = nullptr;

	AddModule(InModuleName);
	std::shared_ptr<FModuleInfo> ModuleInfo = Modules.at(InModuleName);
	if (ModuleInfo->Module)
	{
		LoadedModule = ModuleInfo->Module.get();
	}
	else
	{
		HINSTANCE hModule = LoadLibrary(ModuleInfo->Filename.c_str());
		ModuleInfo->Handle = hModule;

		FInitializeModuleFunctionPtr InitializeModuleFunctionPtr = (FInitializeModuleFunctionPtr)GetProcAddress((HINSTANCE)ModuleInfo->Handle, "InitializeModule");
		ModuleInfo->Module = std::unique_ptr<IModuleInterface>(InitializeModuleFunctionPtr());
		LoadedModule = ModuleInfo->Module.get();
	}
	return LoadedModule;
}

FModuleManager::FModuleManager()
{

}

void FModuleManager::AddModuleToModulesList(const std::string& InModuleName, std::shared_ptr<FModuleInfo>& ModuleInfo)
{
	Modules.insert(std::make_pair(InModuleName, ModuleInfo));
}

std::shared_ptr<FModuleManager::FModuleInfo> FModuleManager::FindModule(const std::string& InModuleName)
{
	std::shared_ptr<FModuleInfo> Result = nullptr;

	if (Modules.find(InModuleName) != Modules.end())
	{
		Result = Modules[InModuleName];
	}
	return Result;
}

void FModuleManager::FindModulePaths(const char *NamePattern, std::map<std::string, std::string> &OutModulePaths, bool bCanUseCache /*= true*/) const
{
	if (bCanUseCache)
	{
		auto it = ModulePathsCache.find(NamePattern);
		if (it != ModulePathsCache.end())
		{
			OutModulePaths.insert(std::make_pair(NamePattern, it->second));
			return;
		}
	}

	ModulePathsCache.insert(std::make_pair(std::string("Core"), std::string("libx0.dll")));
	ModulePathsCache.insert(std::make_pair(std::string("Renderer"), std::string("libx4.dll")));
	ModulePathsCache.insert(std::make_pair(std::string("Engine"), std::string("libx5.dll")));

	FindModulePaths(NamePattern, OutModulePaths);
}

int32 FModuleManager::FModuleInfo::CurrentLoadOrder = -1;
