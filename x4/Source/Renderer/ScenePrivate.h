#pragma once

#include "SceneInterface.h"

class FPrimitiveSceneInfo;

class FScene : public FSceneInterface
{
	virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
	virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;

	void AddPrimitiveSceneInfo_RenderThread(FPrimitiveSceneInfo* PrimitiveSceneInfo);
};
