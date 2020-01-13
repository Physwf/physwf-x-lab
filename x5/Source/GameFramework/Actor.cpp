#include "GameFramework/Actor.h"
#include "Level.h"
#include "Components/StaticMeshComponet.h"


AActor::AActor(ULevel* InOwner):Owner(InOwner)
{
	Component = new UStaticMeshComponent(this);
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
	return Owner->OwningWorld;
}

