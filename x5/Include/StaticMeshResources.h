#pragma once

#include "X5.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Rendering/ColorVertexBuffer.h"
#include "PrimitiveSceneProxy.h"
#include "LocalVertexFactory.h"
#include "RawIndexBuffer.h"

class UStaticMeshComponent;
class FStaticMeshLODGroup
{
};

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

	void Cache(class UStaticMesh* Owner);

	X5_API void AllocateLODResources(int32 NumLODs);
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
	virtual void SetIndexSource(int32 LODIndex, int32 ElementIndex, FMeshBatch& OutMeshElement/*, bool bWireframe, bool bRequiresAdjacencyInformation, bool bUseInversedIndices, bool bAllowPreCulledIndices*/) const;

protected:
	/** Information used by the proxy about a single LOD of the mesh. */
	class FLODInfo
	{
	public:

		/** Information about an element of a LOD. */
		struct FSectionInfo
		{
			/** Default constructor. */
			FSectionInfo()
				: /*Material(NULL)*/
				/*,*/ FirstPreCulledIndex(0)
				, NumPreCulledTriangles(-1)
			{}

			/** The material with which to render this section. */
			//UMaterialInterface* Material;
			int32 FirstPreCulledIndex;
			int32 NumPreCulledTriangles;
		};

		/** Per-section information. */
		std::vector<FSectionInfo> Sections;

		/** Vertex color data for this LOD (or NULL when not overridden), FStaticMeshComponentLODInfo handle the release of the memory */
		FColorVertexBuffer* OverrideColorVertexBuffer;

		//TUniformBufferRef<FLocalVertexFactoryUniformShaderParameters> OverrideColorVFUniformBuffer;

		const FRawStaticIndexBuffer* PreCulledIndexBuffer;

		/** Initialization constructor. */
		FLODInfo(const UStaticMeshComponent* InComponent, const std::vector<FStaticMeshVertexFactories*>& InLODVertexFactories, int32 InLODIndex/*, bool bLODsShareStaticLighting*/);

		//bool UsesMeshModifyingMaterials() const { return bUsesMeshModifyingMaterials; }

		// FLightCacheInterface.
		//virtual FLightInteraction GetInteraction(const FLightSceneProxy* LightSceneProxy) const override;

	private:
		//TArray<FGuid> IrrelevantLights;

		/** True if any elements in this LOD use mesh-modifying materials **/
		bool bUsesMeshModifyingMaterials;
	};

	FStaticMeshRenderData* RenderData;

	std::vector<FLODInfo*> LODs;

	int32 ClampedMinLOD;

public:
	float GetScreenSize(int32 LODIndex) const;

};

