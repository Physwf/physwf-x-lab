#pragma once

#include "D3D11RHI.h"
#include "Math.h"

#include <vector>

struct MeshBatchElement
{
	const ID3D11Buffer* IndexBuffer;

	uint32 FirstIndex;
	uint32 NumPrimitives;

	FMeshBatchElement()
		: IndexBuffer(nullptr)
	{

	}
};

struct MeshBatch
{
	std::vector<MeshBatchElement> Elements;
};