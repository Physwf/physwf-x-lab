#include "Mesh.h"

class PositionVertexData : public TMeshVertexData<PostionVertex>
{

};

void PostionVertexBuffer::Init(uint32 InNumVertices)
{
	NumberVertices = InNumVertices;

	AllocateData();

	VertexData->ResizeBuffer(NumberVertices);

	Data = NumberVertices > 0 ? VertexData->GetDataPointer() : NULL;

}

void PostionVertexBuffer::Init(const std::vector<FVector>& InPostions)
{
	NumberVertices = InPostions.size();
	if (NumberVertices)
	{
		AllocateData();
		VertexData->ResizeBuffer(NumberVertices);
		Data = VertexData->GetDataPointer();
		FMemory::Memcpy(Data, InPostions.data(), Stride*NumberVertices);
	}
}

void PostionVertexBuffer::CleanUp()
{
	if (VertexData)
	{
		delete VertexData;
		VertexData = NULL;
	}
}

void PostionVertexBuffer::InitRHI()
{
	if (VertexData->GetResourceSize() > 0)
	{
		APIResourceCreateInfo CreateInfo(VertexData->GetDataPointer());
		Buffer = CreateVertexBuffer(VertexData->GetResourceSize(), 0, CreateInfo);
	}
}

void PostionVertexBuffer::AllocateData()
{
	CleanUp();

	VertexData = new PositionVertexData();
	Stride = VertexData->GetStride();
}

RawIndexBuffer::RawIndexBuffer()
{

}

void RawIndexBuffer::SetIndices(const std::vector<uint32> InIndices)
{
	IndexData.clear();
	IndexData.resize(InIndices.size());
	FMemory::Memcpy(IndexData.data(), InIndices.data(), InIndices.size() * sizeof(uint32));
}

uint32 RawIndexBuffer::GetNumIndices() const
{
	return IndexData.size();
}

uint32 RawIndexBuffer::GetAllocatedSize() const
{
	return IndexData.size() * sizeof(uint32);
}

void RawIndexBuffer::Discard()
{
	IndexData.clear();
}

void RawIndexBuffer::InitRHI()
{
	uint32 Size = GetAllocatedSize();
	if (Size > 0)
	{
		APIResourceCreateInfo CreateInfo(IndexData.data());
		Buffer = CreateIndexBuffer(Size, 0, CreateInfo);
	}
}

void LocalVertexFactory::InitRHI()
{

}

void LocalVertexFactory::SetData(const MeshDataType& InData)
{
	Data = InData;
	UpdateRHI();
}

bool SimpleMesh::GetMeshElement(int32 BatchIndex, int32 SectionIndex, MeshBatch& OutMeshBatch)
{
	OutMeshBatch.VF = VF;
	const StaticMeshSection& Section = Sections[SectionIndex];

	SetIndexSource(SectionIndex, OutMeshBatch, false, false, false, false);

	MeshBatchElement& OutBatchElement = OutMeshBatch.Elements[0];

	return true;
}

void SimpleMesh::SetIndexSource(int32 ElementIndex, FMeshBatch& OutMeshElement, bool bWireframe, bool bRequiresAdjacencyInformation, bool bUseInversedIndices, bool bAllowPreCulledIndices) const
{
	MeshBatchElement& OutElement = OutMeshElement.Elements[0];
	const StaticMeshSection& Section = Sections[SectionIndex];
	OutElement.IndexBuffer = &IB;
	OutElement.FirstIndex = Section.FirstIndex;
	OutElement.NumPrimitives = Section.NumTriangles;
}
