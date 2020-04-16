#include "EngineModule.h"
#include "Modules/ModuleManager.h"

IRendererModule* CachedRendererModule = nullptr;

X5_API IRendererModule& GetRendererModule()
{
	if (!CachedRendererModule)
	{
		CachedRendererModule = &FModuleManager::LoadModuleChecked<IRendererModule>(L"Renderer");
	}
	return *CachedRendererModule;
}
