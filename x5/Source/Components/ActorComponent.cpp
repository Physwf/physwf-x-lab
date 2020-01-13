#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"

UActorComponent::UActorComponent(AActor* InOwner)
	:Owner(InOwner)
{

}

void UActorComponent::RegisterComponent()
{
	AActor* MyOwner = GetOwner();
	UWorld* MyOwnerWorld = MyOwner ? MyOwner->GetWorld() : nullptr;
	RegisterComponentWithWorld(MyOwnerWorld);
}

void UActorComponent::UnregisterComponent()
{
	ExecuteUnregisterEvents();
}

void UActorComponent::RegisterComponentWithWorld(UWorld* InWorld)
{
	WorldPrivate = InWorld;
	ExecuteRegisterEvents();
}

AActor* UActorComponent::GetOwner() const
{
	return Owner;
}

void UActorComponent::CreateRenderState_Concurrent()
{

}

void UActorComponent::DestroyRenderState_Concurrent()
{

}

void UActorComponent::ExecuteUnregisterEvents()
{
	DestroyRenderState_Concurrent();
}

void UActorComponent::ExecuteRegisterEvents()
{
	CreateRenderState_Concurrent();
}

