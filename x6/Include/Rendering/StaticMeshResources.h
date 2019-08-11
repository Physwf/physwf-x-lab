#pragma once

#include "CoreTypes.h"

struct XStaticMeshSection
{
	int32 MaterialIndex;

	uint32 FirstIndex;
	uint32 NumTriangles;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;
};