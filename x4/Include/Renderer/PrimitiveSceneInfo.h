#pragma once

class FPrimitiveSceneProxy;
class FScene;
class UPrimitiveComponent;

class FPrimitiveSceneInfo 
{
public:
	FPrimitiveSceneProxy* Proxy;

	FScene* Scene;

	FPrimitiveSceneInfo(UPrimitiveComponent* InPrimitive, FScene* InScene);

	~FPrimitiveSceneInfo();

	void AddToScene(bool bUpdateStaticDrawLists, bool bAddToStaticDrawLists = true);

	void AddStaticMeshes(bool bUpdateStaticDrawLists = true);
};