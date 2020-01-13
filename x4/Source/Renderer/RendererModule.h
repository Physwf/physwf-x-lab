#pragma once

#include "RenderCore/RendererInterface.h"

class FRendererModule : public IRendererModule
{
public:
	FRendererModule();

	virtual FSceneInterface* AllocateScene(class UWorld* World) override;
	virtual void RemoveScene(FSceneInterface* Scene) override;
};

