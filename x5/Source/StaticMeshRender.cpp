#include "StaticMeshResources.h"
#include "Components/StaticMeshComponet.h"
#include "SceneManagement.h"

FStaticMeshSceneProxy::FStaticMeshSceneProxy(UStaticMeshComponent* Component, bool bForceLODsShareStaticLighting):
	FPrimitiveSceneProxy(Component)
{

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
	return true;
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


float FStaticMeshSceneProxy::GetScreenSize(int32 LODIndex) const
{
	return 0.0f;
	//return RenderData->ScreenSize[LODIndex].GetValueForFeatureLevel(GetScene().GetFeatureLevel());
}




