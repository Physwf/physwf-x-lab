#pragma once

class FStaticMeshBuilder
{
public:
	FStaticMeshBuilder();

	bool Build(class FStaticMeshRenderData& OutRenderData, class UStaticMesh* StaticMesh);
	virtual ~FStaticMeshBuilder() {}

};
