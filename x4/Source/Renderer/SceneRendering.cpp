#include "RendererModule.h"

extern "C" __declspec(dllexport) IModuleInterface* InitializeModule()
{
	return new FRendererModule();
}



FRendererModule::FRendererModule()
{

}

FSceneInterface* FRendererModule::AllocateScene(class UWorld* World)
{
	return nullptr;
}

void FRendererModule::RemoveScene(FSceneInterface* Scene)
{

}
