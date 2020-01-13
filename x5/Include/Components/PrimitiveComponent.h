#pragma once

#include "ActorComponent.h"

class FPrimitiveSceneProxy;
class AActor;

class UPrimitiveComponent : public UActorComponent
{
public:
	UPrimitiveComponent(AActor* InOwner);
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