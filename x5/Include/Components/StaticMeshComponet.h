#pragma once

#include "PrimitiveComponent.h"
#include "StaticMesh.h"

class UStaticMeshComponent : public UPrimitiveComponent
{
	UStaticMesh* StaticMesh;
};