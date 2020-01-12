#include "World.h"
#include "GameFramework/Actor.h"
#include "Level.h"

AActor* UWorld::SpawnActor()
{
	AActor* NewActor = new AActor();
	CurrentLevel->Actors.push_back(NewActor);
	NewActor->PostSpawnActor();
	return NewActor;
}

void UWorld::InitWorld()
{
	CurrentLevel = new ULevel();
	AActor* NewActor = SpawnActor();
}

void UWorld::InitializeNewWorld()
{
	InitWorld();
}

UWorld* UWorld::CreateWorld()
{
	UWorld* NewWorld = new UWorld();
	NewWorld->InitializeNewWorld();
	return NewWorld;
}

void UWorld::DestroyWorld()
{

}

