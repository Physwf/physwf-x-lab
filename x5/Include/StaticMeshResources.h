#pragma once

#include "Rendering/StaticMeshVertexBuffer.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Rendering/ColorVertexBuffer.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"
#include "RawIndexBuffer.h"

class UStaticMeshComponent;

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
	FPositionVertexBuffer PositionVertexBuffer;
	FColorVertexBuffer ColorVertexBuffer;
};

struct FStaticMeshLODResources
{
	FStaticMeshVertexBuffers VertexBuffers;
	FRawStaticIndexBuffer IndexBuffer;

	std::vector<FStaticMeshSection> Sections;
};

struct FStaticMeshVertexFactories
{
	FLocalVertexFactory VertexFactory;
};

class FStaticMeshRenderData
{
public:
	std::vector<FStaticMeshLODResources*> LODResources;
	std::vector<FStaticMeshVertexFactories*> LODVertexFactories;
};

/**
 * A static mesh component scene proxy.
 */
class FStaticMeshSceneProxy : public FPrimitiveSceneProxy
{
public:
	FStaticMeshSceneProxy(UStaticMeshComponent* Component, bool bForceLODsShareStaticLighting);

	virtual ~FStaticMeshSceneProxy();

	virtual int32 GetNumMeshBatches() const
	{
		return 1;
	}

	virtual bool GetMeshElement(
		int32 LODIndex,
		int32 BatchIndex,
		int32 ElementIndex,
		/*uint8 InDepthPriorityGroup,*/
		/*bool bUseSelectedMaterial,*/
		/*bool bUseHoveredMaterial,*/
		/*bool bAllowPreCulledIndices,*/
		FMeshBatch& OutMeshBatch) const;


	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) override;

protected:
	FStaticMeshRenderData* RenderData;

	int32 ClampedMinLOD;

public:
	float GetScreenSize(int32 LODIndex) const;
};
