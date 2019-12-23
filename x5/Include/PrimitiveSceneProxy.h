#pragma once

#include <vector>
#include "CoreTypes.h"
#include "MeshBatch.h"

class FStaticPrimitiveDrawInterface;
class UPrimitiveComponent;

class FPrimitiveSceneProxy
{
public:
	FPrimitiveSceneProxy(const UPrimitiveComponent* InComponent);

	/** Virtual destructor. */
	virtual ~FPrimitiveSceneProxy();


	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) {}

	/** Gathers a description of the mesh elements to be rendered for the given LOD index, without consideration for views. */
	virtual void GetMeshDescription(int32 LODIndex, std::vector<FMeshBatch>& OutMeshElements) const {}
};