#include "Components/StaticMeshComponet.h"
#include "StaticMeshResources.h"
#include "FbxImporter/FbxImporter.h"

UStaticMeshComponent::UStaticMeshComponent(AActor* InOwner): UPrimitiveComponent(InOwner)
{
	FFbxImporter Importer;
	StaticMesh = Importer.ImportStaticMesh("./k526efluton4-House_15/247_House 15_fbx.FBX");
	StaticMesh->Build();
}

FPrimitiveSceneProxy* UStaticMeshComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = ::new FStaticMeshSceneProxy(this, false);
	return Proxy;
}

