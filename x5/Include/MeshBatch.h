#pragma once

#include "ShaderCore/VertexFactory.h"

#include <vector>

struct FMeshBatchElement
{
	union
	{
		/** If bIsSplineProxy, Instance runs, where number of runs is specified by NumInstances.  Run structure is [StartInstanceIndex, EndInstanceIndex]. */
		uint32* InstanceRuns;
	};

	uint32 NumPrimitives;
	uint32 NumInstances;
	uint8 bIsInstanceRuns : 1;
};

struct FMeshBatch
{
	std::vector<FMeshBatchElement> Elements;

	inline int32 GetNumPrimitives() const
	{
		int32 Count = 0;
		for (uint32 ElementIdx = 0; ElementIdx < Elements.size(); ElementIdx++)
		{
			if (Elements[ElementIdx].bIsInstanceRuns && Elements[ElementIdx].InstanceRuns)
			{
				for (uint32 Run = 0; Run < Elements[ElementIdx].NumInstances; Run++)
				{
					Count += Elements[ElementIdx].NumPrimitives * (Elements[ElementIdx].InstanceRuns[Run * 2 + 1] - Elements[ElementIdx].InstanceRuns[Run * 2] + 1);
				}
			}
			else
			{
				Count += Elements[ElementIdx].NumPrimitives * Elements[ElementIdx].NumInstances;
			}
		}
		return Count;
	}

	const FVertexFactory* VertexFactory;
};