#pragma once

#include "SceneInterface.h"

class IRendererModule
{
	virtual FSceneInterface* AllocateScene(class UWorld* World) = 0;
	virtual void RemoveScene(FSceneInterface* Scene) = 0;
};