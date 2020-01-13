#include "RendererModule.h"
#include "Renderer/ScenePrivate.h"

extern "C" __declspec(dllexport) IModuleInterface* InitializeModule()
{
	return new FRendererModule();
}



FRendererModule::FRendererModule()
{

}

FSceneInterface* FRendererModule::AllocateScene(class UWorld* World)
{
	FScene* NewScene = new FScene(World);
	return NewScene;
}

void FRendererModule::RemoveScene(FSceneInterface* Scene)
{

}
