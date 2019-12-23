#pragma once

#include "Renderer/PrimitiveSceneInfo.h"
#include "MeshBatch.h"

class FStaticMesh : public FMeshBatch
{
public:
	float ScreenSize;

	FPrimitiveSceneInfo* PrimitiveSceneInfo;

	uint32 Id;

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

	void AddToDrawLists(FScene* Scene);
};