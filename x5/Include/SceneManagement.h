#pragma once

#include "MeshBatch.h"
/**
 * An interface used to query a primitive for its static elements.
 */
class FStaticPrimitiveDrawInterface
{
public:
	virtual ~FStaticPrimitiveDrawInterface() { }
	//virtual void SetHitProxy(HHitProxy* HitProxy) = 0;
	virtual void DrawMesh(const FMeshBatch& Mesh, float ScreenSize) = 0;
};
