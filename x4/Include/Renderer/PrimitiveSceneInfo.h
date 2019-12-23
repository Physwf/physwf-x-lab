#pragma once

#include <vector>

class FPrimitiveSceneProxy;
class FScene;
class UPrimitiveComponent;

class FPrimitiveSceneInfo 
{
public:
	FPrimitiveSceneProxy* Proxy;

	std::vector<class FStaticMesh*> StaticMeshes;

	FScene* Scene;

	FPrimitiveSceneInfo(UPrimitiveComponent* InPrimitive, FScene* InScene);

	~FPrimitiveSceneInfo();

	void AddToScene(bool bUpdateStaticDrawLists, bool bAddToStaticDrawLists = true);

	void AddStaticMeshes(bool bUpdateStaticDrawLists = true);
};