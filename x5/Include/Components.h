#pragma once

#include "Math/Vector.h"

// Constants.
enum { MAX_TEXCOORDS = 4, MAX_STATIC_TEXCOORDS = 8 };

/** The information used to build a static-mesh vertex. */
struct FStaticMeshBuildVertex
{
	FVector Position;

	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;

	FVector2D UVs[MAX_STATIC_TEXCOORDS];
	FColor Color;
};