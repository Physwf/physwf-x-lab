#include "Components/StaticMeshComponet.h"
#include "StaticMeshResources.h"

FPrimitiveSceneProxy* UStaticMeshComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = ::new FStaticMeshSceneProxy(this, false);
	return Proxy;
}

