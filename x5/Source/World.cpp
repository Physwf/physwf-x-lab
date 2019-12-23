#include "World.h"
#include "Actor.h"

AActor* UWorld::SpawnActor()
{
	AActor* NewActor = new AActor();
	CurrentLevel->Actors.push_back(NewActor);
	NewActor->PostSpawnActor();
	return NewActor;
}

