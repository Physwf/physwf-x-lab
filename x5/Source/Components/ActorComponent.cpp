#include "ActorComponent.h"

void UActorComponent::RegisterComponent()
{
	RegisterComponentWithWorld();
}

void UActorComponent::UnregisterComponent()
{
	ExecuteUnregisterEvents();
}

void UActorComponent::RegisterComponentWithWorld(/*UWorld* InWorld*/)
{
	ExecuteRegisterEvents();
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

