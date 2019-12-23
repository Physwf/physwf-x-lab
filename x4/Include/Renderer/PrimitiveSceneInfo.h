#pragma once

class FPrimitiveSceneProxy;

class FPrimitiveSceneInfo 
{
public:
	FPrimitiveSceneProxy* Proxy;

	void AddStaticMeshes(bool bUpdateStaticDrawLists = true);
};