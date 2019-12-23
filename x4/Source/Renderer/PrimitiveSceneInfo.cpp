#include "Renderer/PrimitiveSceneInfo.h"
#include "Components/PrimitiveComponent.h"
#include "SceneManagement.h"
#include "ScenePrivate.h"
#include "PrimitiveSceneProxy.h"
#include "Misc/AssertionMacros.h"
#include "SceneCore.h"

class FBatchingSPDI : public FStaticPrimitiveDrawInterface
{
public:
	FBatchingSPDI(FPrimitiveSceneInfo* InPrimitiveSceneInfo) :
		PrimitiveSceneInfo(InPrimitiveSceneInfo)
	{}

	virtual void DrawMesh(const FMeshBatch& Mesh, float ScreenSize) final override
	{
		if (Mesh.GetNumPrimitives() > 0)
		{
			check(Mesh.VertexFactory);
			check(Mesh.VertexFactory->IsInitialized());

			//PrimitiveSceneInfo->Proxy->VerifyUsedMaterial(Mesh.MaterialRenderProxy);

			FStaticMesh* StaticMesh = new FStaticMesh(
				PrimitiveSceneInfo,
				Mesh,
				ScreenSize
			);
			PrimitiveSceneInfo->StaticMeshes.push_back(StaticMesh);
		}
	}
private:
	FPrimitiveSceneInfo* PrimitiveSceneInfo;
};

FPrimitiveSceneInfo::FPrimitiveSceneInfo(UPrimitiveComponent* InPrimitive, FScene* InScene):
	Proxy(InPrimitive->SceneProxy),
	Scene(InScene)
{
}

FPrimitiveSceneInfo::~FPrimitiveSceneInfo()
{

}

void FPrimitiveSceneInfo::AddToScene(bool bUpdateStaticDrawLists, bool bAddToStaticDrawLists /*= true*/)
{
	if (bUpdateStaticDrawLists)
	{
		AddStaticMeshes(bAddToStaticDrawLists);
	}
}

void FPrimitiveSceneInfo::AddStaticMeshes(bool bUpdateStaticDrawLists /*= true*/)
{
	FBatchingSPDI BatchingSPDI(this);
	Proxy->DrawStaticElements(&BatchingSPDI);
}

