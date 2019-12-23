#include "StaticMeshResources.h"
#include "Components/StaticMeshComponet.h"
#include "SceneManagement.h"
#include "RHI/RHIDefinitions.h"
#include "Misc/AssertionMacros.h"

FStaticMeshSceneProxy::FStaticMeshSceneProxy(UStaticMeshComponent* InComponent, bool bForceLODsShareStaticLighting):
	FPrimitiveSceneProxy(InComponent),
	RenderData(InComponent->GetStaticMesh()->RenderData.get())
{
	check(RenderData);

	LODs.reserve(RenderData->LODResources.size());

	for (uint32 LODIndex = 0; LODIndex < RenderData->LODResources.size(); LODIndex++)
	{
		FLODInfo* NewLODInfo = new FLODInfo(InComponent, RenderData->LODVertexFactories, LODIndex/*, bLODsShareStaticLighting*/);
		LODs.push_back(NewLODInfo);
	}
}

FStaticMeshSceneProxy::~FStaticMeshSceneProxy()
{

}

bool FStaticMeshSceneProxy::GetMeshElement(int32 LODIndex, int32 BatchIndex, int32 SectionIndex, /*uint8 InDepthPriorityGroup, bool bUseSelectedMaterial, bool bUseHoveredMaterial, bool bAllowPreCulledIndices,*/ FMeshBatch& OutMeshBatch) const
{
	const FStaticMeshLODResources& LOD = *RenderData->LODResources[LODIndex];
	const FStaticMeshVertexFactories& VFs = *RenderData->LODVertexFactories[LODIndex];
	const FStaticMeshSection& Section = LOD.Sections[SectionIndex];

	OutMeshBatch.VertexFactory = &VFs.VertexFactory;

	SetIndexSource(LODIndex, SectionIndex, OutMeshBatch);

	FMeshBatchElement& OutBatchElement = OutMeshBatch.Elements[0];

	if (OutBatchElement.NumPrimitives > 0)
	{
		//OutMeshBatch.LCI = &ProxyLODInfo;
		//OutBatchElement.PrimitiveUniformBufferResource = &GetUniformBuffer();
		OutBatchElement.MinVertexIndex = Section.MinVertexIndex;
		OutBatchElement.MaxVertexIndex = Section.MaxVertexIndex;
		OutMeshBatch.LODIndex = LODIndex;

		//OutMeshBatch.ReverseCulling = (bReverseCulling || IsLocalToWorldDeterminantNegative()) && !bUseReversedIndices;
		//OutMeshBatch.CastShadow = bCastShadow && Section.bCastShadow;
		//OutMeshBatch.DepthPriorityGroup = (ESceneDepthPriorityGroup)InDepthPriorityGroup;

		return true;
	}
	return false;

}

void FStaticMeshSceneProxy::DrawStaticElements(FStaticPrimitiveDrawInterface* PDI)
{
	int32 NumLODs = (int32)RenderData->LODResources.size();
	for (int32 LODIndex = ClampedMinLOD; LODIndex < NumLODs; LODIndex++)
	{
		const FStaticMeshLODResources& LODModel = *RenderData->LODResources[LODIndex];
		float ScreenSize = GetScreenSize(LODIndex);

		// Draw the static mesh elements.
		for (int32 SectionIndex = 0; SectionIndex < (int32)LODModel.Sections.size(); SectionIndex++)
		{
			const int32 NumBatches = GetNumMeshBatches();

			for (int32 BatchIndex = 0; BatchIndex < NumBatches; BatchIndex++)
			{
				FMeshBatch MeshBatch;

				if (GetMeshElement(LODIndex, BatchIndex, SectionIndex,/* PrimitiveDPG, bUseSelectedMaterial, bUseHoveredMaterial, true,*/ MeshBatch))
				{
					// If we have submitted an optimized shadow-only mesh, remaining mesh elements must not cast shadows.
					//MeshBatch.CastShadow &= !bUseUnifiedMeshForShadow;
					//MeshBatch.bUseAsOccluder &= !bUseUnifiedMeshForDepth;

					PDI->DrawMesh(MeshBatch, ScreenSize);
				}
			}
		}
	}
}

void FStaticMeshSceneProxy::SetIndexSource(int32 LODIndex, int32 SectionIndex, FMeshBatch& OutMeshElement/*, bool bWireframe, bool bRequiresAdjacencyInformation, bool bUseInversedIndices, bool bAllowPreCulledIndices*/) const
{
	FMeshBatchElement& OutElement = OutMeshElement.Elements[0];
	const FStaticMeshLODResources& LODModel = *RenderData->LODResources[LODIndex];

	const FStaticMeshSection& Section = LODModel.Sections[SectionIndex];
	OutMeshElement.Type = PT_TriangleList;

	OutElement.IndexBuffer = &LODModel.IndexBuffer;
	OutElement.FirstIndex = Section.FirstIndex;
	OutElement.NumPrimitives = Section.NumTriangles;
}


float FStaticMeshSceneProxy::GetScreenSize(int32 LODIndex) const
{
	return 0.0f;
	//return RenderData->ScreenSize[LODIndex].GetValueForFeatureLevel(GetScene().GetFeatureLevel());
}

FStaticMeshSceneProxy::FLODInfo::FLODInfo(const UStaticMeshComponent* InComponent, const std::vector<FStaticMeshVertexFactories*>& InLODVertexFactories, int32 InLODIndex/*, bool bLODsShareStaticLighting*/)
{

}


