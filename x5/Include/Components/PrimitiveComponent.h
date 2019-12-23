#pragma once

#include "ActorComponent.h"

class FPrimitiveSceneProxy;

class UPrimitiveComponent : public UActorComponent
{
public:
	FPrimitiveSceneProxy* SceneProxy;
protected:
	virtual void CreateRenderState_Concurrent() override;
public:
	virtual FPrimitiveSceneProxy* CreateSceneProxy()
	{
		return nullptr;
	}
};