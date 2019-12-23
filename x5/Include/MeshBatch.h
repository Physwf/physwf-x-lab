#pragma once

#include "ShaderCore/VertexFactory.h"
#include "RHI/RHIDefinitions.h"

#include <vector>

struct FMeshBatchElement
{
	const FIndexBuffer* IndexBuffer;

	union
	{
		/** If bIsSplineProxy, Instance runs, where number of runs is specified by NumInstances.  Run structure is [StartInstanceIndex, EndInstanceIndex]. */
		uint32* InstanceRuns;
	};

	uint32 FirstIndex;
	uint32 NumPrimitives;
	/** Number of instances to draw.  If InstanceRuns is valid, this is actually the number of runs in InstanceRuns. */
	uint32 NumInstances;
	uint32 BaseVertexIndex;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;
	// Meaning depends on the vertex factory, e.g. FGPUSkinPassthroughVertexFactory: element index in FGPUSkinCache::CachedElements
	void* VertexFactoryUserData;
	int32 UserIndex;
	float MinScreenSize;
	float MaxScreenSize;

	uint8 InstancedLODIndex : 4;
	uint8 InstancedLODRange : 4;
	uint8 bUserDataIsColorVertexBuffer : 1;
	uint8 bIsInstancedMesh : 1;
	uint8 bIsSplineProxy : 1;
	uint8 bIsInstanceRuns : 1;

	FMeshBatchElement()
		: /*PrimitiveUniformBufferResource(nullptr)*/
		/*,*/ IndexBuffer(nullptr)
		, InstanceRuns(nullptr)
		/*, UserData(nullptr)*/
		, NumInstances(1)
		, BaseVertexIndex(0)
		, VertexFactoryUserData(nullptr)
		, UserIndex(-1)
		, MinScreenSize(0.0f)
		, MaxScreenSize(1.0f)
		, InstancedLODIndex(0)
		, InstancedLODRange(0)
		, bUserDataIsColorVertexBuffer(false)
		, bIsInstancedMesh(false)
		, bIsSplineProxy(false)
		, bIsInstanceRuns(false)
	{
	}
};

struct FMeshBatch
{
	std::vector<FMeshBatchElement> Elements;
	int8 LODIndex;

	/** Conceptual HLOD index used for the HLOD Coloration visualization. */
	//int8 VisualizeHLODIndex;

	//uint32 ReverseCulling : 1;
	//uint32 bDisableBackfaceCulling : 1;
	//uint32 CastShadow : 1;				// Wheter it can be used in shadow renderpasses.
	//uint32 bUseForMaterial : 1;	// Whether it can be used in renderpasses requiring material outputs.
	//uint32 bUseAsOccluder : 1;			// Whether it can be used in renderpasses only depending on the raw geometry (i.e. Depth Prepass).
	//uint32 bWireframe : 1;
	// e.g. PT_TriangleList(default), PT_LineList, ..
	uint32 Type : PT_NumBits;
	// e.g. SDPG_World (default), SDPG_Foreground
	//uint32 DepthPriorityGroup : SDPG_NumBits;

	/** Whether view mode overrides can be applied to this mesh eg unlit, wireframe. */
	//uint32 bCanApplyViewModeOverrides : 1;

	/**
	 * Whether to treat the batch as selected in special viewmodes like wireframe.
	 * This is needed instead of just Proxy->IsSelected() because some proxies do weird things with selection highlighting, like FStaticMeshSceneProxy.
	 */
	//uint32 bUseWireframeSelectionColoring : 1;

	/**
	 * Whether the batch should receive the selection outline.
	 * This is useful for proxies which support selection on a per-mesh batch basis.
	 * They submit multiple mesh batches when selected, some of which have bUseSelectionOutline enabled.
	 */
	//uint32 bUseSelectionOutline : 1;

	/** Whether the mesh batch can be selected through editor selection, aka hit proxies. */
	//uint32 bSelectable : 1;

	/** Whether the mesh batch needs VertexFactory->GetStaticBatchElementVisibility to be called each frame to determine which elements of the batch are visible. */
	//uint32 bRequiresPerElementVisibility : 1;

	/** Whether the mesh batch should apply dithered LOD. */
	//uint32 bDitheredLODTransition : 1;

	/** If bDitheredLODTransition and this is a dynamic mesh element, then this is the alpha for dither fade (static draw lists need to derive this later as it is changes every frame) */
	//float DitheredLODTransitionAlpha;

	// can be NULL
	//const FLightCacheInterface* LCI;

	/** Vertex factory for rendering, required. */
	const FVertexFactory* VertexFactory;

	inline int32 GetNumPrimitives() const
	{
		int32 Count = 0;
		for (uint32 ElementIdx = 0; ElementIdx < Elements.size(); ElementIdx++)
		{
			if (Elements[ElementIdx].bIsInstanceRuns && Elements[ElementIdx].InstanceRuns)
			{
				for (uint32 Run = 0; Run < Elements[ElementIdx].NumInstances; Run++)
				{
					Count += Elements[ElementIdx].NumPrimitives * (Elements[ElementIdx].InstanceRuns[Run * 2 + 1] - Elements[ElementIdx].InstanceRuns[Run * 2] + 1);
				}
			}
			else
			{
				Count += Elements[ElementIdx].NumPrimitives * Elements[ElementIdx].NumInstances;
			}
		}
		return Count;
	}
};