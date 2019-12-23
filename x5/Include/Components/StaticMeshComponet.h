#pragma once

#include "PrimitiveComponent.h"
#include "StaticMesh.h"

class UStaticMeshComponent : public UPrimitiveComponent
{
	
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