#include "Components/PrimitiveComponent.h"
#include "World.h"
#include "SceneInterface.h"

UPrimitiveComponent::UPrimitiveComponent(AActor* InOwner) :UActorComponent(InOwner)
{

}

void UPrimitiveComponent::CreateRenderState_Concurrent()
{
	GetWorld()->Scene->AddPrimitive(this);
}

