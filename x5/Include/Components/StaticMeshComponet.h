#pragma once

#include "PrimitiveComponent.h"
#include "StaticMesh.h"

class AActor;

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
	UStaticMeshComponent(AActor* InOwner);
protected:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
private:
	UStaticMesh* StaticMesh;
public:
	UStaticMesh* GetStaticMesh() const
	{
		return StaticMesh;
	}
};