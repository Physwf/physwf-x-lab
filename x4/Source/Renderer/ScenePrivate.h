#pragma once

#include <vector>
#include <unordered_map>

#include "SceneInterface.h"

class FPrimitiveSceneInfo;

class FScene : public FSceneInterface
{
public:
	virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
	virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;

	void AddPrimitiveSceneInfo_RenderThread(FPrimitiveSceneInfo* PrimitiveSceneInfo);

	/** Packed array of primitives in the scene. */
	std::vector<FPrimitiveSceneInfo*> Primitives;
	/** Packed array of primitive scene proxies in the scene. */
	std::vector<FPrimitiveSceneProxy*> PrimitiveSceneProxies;

	/** The static meshes in the scene. */
	std::vector<FStaticMesh*> StaticMeshes;
};
