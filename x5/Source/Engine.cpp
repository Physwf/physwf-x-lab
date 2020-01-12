#include "Engine.h"

#include "GameInstance.h"
#include "World.h"

X5_API void FWorldContext::SetCurrentWorld(UWorld *World)
{
	ThisCurrentWorld = World;
}

void UEngine::Init()
{
	//(new FFbxImporter())->ImportStaticMesh("./4gikevqde8-IronMan/ironman.FBX", NULL);

}

void UEngine::Start()
{

}

void UEngine::Tick()
{

}

FWorldContext& UEngine::CreateNewWorldContext()
{
	FWorldContext *NewWorldContext = new  FWorldContext;
	WorldList.push_back(NewWorldContext);
	return *NewWorldContext;
}

bool UEngine::Browse(FWorldContext& WorldContext)
{
	return LoadMap(WorldContext);
}

bool UEngine::LoadMap(FWorldContext& WorldContext)
{
	UWorld*	NewWorld = nullptr;
	NewWorld = new UWorld();
	WorldContext.SetCurrentWorld(NewWorld);
	WorldContext.World()->InitWorld();
	return true;
}

