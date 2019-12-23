#include "Components/PrimitiveComponent.h"
#include "World.h"
#include "SceneInterface.h"

void UPrimitiveComponent::CreateRenderState_Concurrent()
{
	GetWorld()->Scene->AddPrimitive(this);
}

