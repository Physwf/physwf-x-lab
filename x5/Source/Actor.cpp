#include "Actor.h"
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
	Component;
}

UWorld* AActor::GetWorld() const
{

}

