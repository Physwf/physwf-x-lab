#pragma once

#include "Rendering/StaticMeshVertexBuffer.h"

struct FStaticMeshSection
{
	/** The index of the material with which to render this section. */
	int32 MaterialIndex;

	/** Range of vertices and indices used when rendering this section. */
	uint32 FirstIndex;
	uint32 NumTriangles;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;

	/** If true, collision is enabled for this section. */
	bool bEnableCollision;
	/** If true, this section will cast a shadow. */
	bool bCastShadow;

	FStaticMeshSection()
		: MaterialIndex(0)
		, FirstIndex(0)
		, NumTriangles(0)
		, MinVertexIndex(0)
		, MaxVertexIndex(0)
		, bEnableCollision(false)
		, bCastShadow(true)
	{

	}
};

struct FStaticMeshVertexBuffers
{
	FStaticMeshVertexBuffer StaticMeshVertexBuffer;

};

struct FStaticMeshLODResources
{
	FStaticMeshVertexBuffers VertexBuffers;
	FRawStaticIndexBuffer IndexBuffer;
};

class FStaticMeshRenderData
{
	std::vector<FStaticMeshLODResources> LODResources;
	//std::vector<FStaticMeshVertexFactories> LODVertexFactories;
};