#pragma once

#include "Renderer/PrimitiveSceneInfo.h"
#include "MeshBatch.h"

class FStaticMesh : public FMeshBatch
{
public:
	FPrimitiveSceneInfo* PrimitiveSceneInfo;
	float ScreenSize;

	FStaticMesh(
		FPrimitiveSceneInfo* InPrimitiveSceneInfo,
		const FMeshBatch& InMesh,
		float InScreenSize
	) :
		FMeshBatch(InMesh),
		ScreenSize(InScreenSize),
		PrimitiveSceneInfo(InPrimitiveSceneInfo)
	{
	}
};