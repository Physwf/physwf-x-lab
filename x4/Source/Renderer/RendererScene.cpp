#include "ScenePrivate.h"
#include "Components/PrimitiveComponent.h"
#include "Renderer/PrimitiveSceneInfo.h"
#include "PrimitiveSceneProxy.h"
#include "World.h"

void FScene::AddPrimitive(UPrimitiveComponent* Primitive)
{
	FPrimitiveSceneProxy* PrimitiveSceneProxy = Primitive->CreateSceneProxy();
	Primitive->SceneProxy = PrimitiveSceneProxy;
	FPrimitiveSceneInfo* PrimitiveSceneInfo = new FPrimitiveSceneInfo(Primitive, this);
	PrimitiveSceneProxy->PrimitiveSceneInfo = PrimitiveSceneInfo;

	AddPrimitiveSceneInfo_RenderThread(PrimitiveSceneInfo);
}

void FScene::RemovePrimitive(UPrimitiveComponent* Primitive)
{

}


void FScene::AddPrimitiveSceneInfo_RenderThread(FPrimitiveSceneInfo* PrimitiveSceneInfo)
{
	PrimitiveSceneInfo->AddToScene(true);
}

FScene::FScene(class UWorld* InWorld):World(InWorld)
{
	World->Scene = this;
}

FScene::~FScene()
{

}