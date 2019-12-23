#include "Renderer/PrimitiveSceneInfo.h"
#include "Components/PrimitiveComponent.h"
#include "SceneManagement.h"
#include "ScenePrivate.h"
#include "PrimitiveSceneProxy.h"

class FBatchingSPDI : public FStaticPrimitiveDrawInterface
{
public:
	FBatchingSPDI(FPrimitiveSceneInfo* InPrimitiveSceneInfo) :
		PrimitiveSceneInfo(InPrimitiveSceneInfo)
	{}

	virtual void DrawMesh(const FMeshBatch& Mesh, float ScreenSize) final override
	{

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

