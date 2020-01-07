#include "Actor.h"
#include "Level.h"
#include "Components/StaticMeshComponet.h"


AActor::AActor()
{
	Component = new UStaticMeshComponent();
}

AActor::~AActor()
{

}

void AActor::PostSpawnActor()
{
	Component->RegisterComponent();
}

UWorld* AActor::GetWorld() const
{
	return LevelPrivite->OwningWorld;
}

