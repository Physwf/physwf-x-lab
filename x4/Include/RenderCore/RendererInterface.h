#pragma once

#include "SceneInterface.h"
#include "Modules/ModuleInterface.h"

class IRendererModule : public IModuleInterface
{
	virtual FSceneInterface* AllocateScene(class UWorld* World) = 0;
	virtual void RemoveScene(FSceneInterface* Scene) = 0;
};