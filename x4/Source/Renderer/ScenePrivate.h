#pragma once

#include <vector>
#include <unordered_map>

#include "SceneInterface.h"
#include "PrimitiveSceneProxy.h"
#include "Renderer/SceneCore.h"

class FPrimitiveSceneInfo;
class UWorld;

class FScene : public FSceneInterface
{
public:
	UWorld* World;

	virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
	virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;

	void AddPrimitiveSceneInfo_RenderThread(FPrimitiveSceneInfo* PrimitiveSceneInfo);

	/** Packed array of primitives in the scene. */
	std::vector<FPrimitiveSceneInfo*> Primitives;
	/** Packed array of primitive scene proxies in the scene. */
	std::vector<FPrimitiveSceneProxy*> PrimitiveSceneProxies;

	/** The static meshes in the scene. */
	std::vector<FStaticMesh*> StaticMeshes;


	FScene(UWorld* InWorld);

	virtual ~FScene();
};



