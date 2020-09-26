#pragma once

struct MeshBatchElement
{
	//const TUniformBuffer<FPrimitiveUniformShaderParameters>* PrimitiveUniformBufferResource;
	//TUniformBufferRef<FPrimitiveUniformShaderParameters> PrimitiveUniformBuffer;

	const FIndexBuffer* IndexBuffer;

	uint32 FirstIndex;
	uint32 NumPrimitives;
};

struct MeshBactch
{
	std::vector<MeshBatchElement> Elements;

	const FVertexFactory* VF;

	//const FMaterialRenderProxy* MaterialRenderProxy;

	MeshBactch()
	{
		Elements.resize(1);
	}
};