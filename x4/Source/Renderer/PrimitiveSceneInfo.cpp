#include "Renderer/PrimitiveSceneInfo.h"
#include "SceneManagement.h"

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

void FPrimitiveSceneInfo::AddStaticMeshes(bool bUpdateStaticDrawLists /*= true*/)
{

}

