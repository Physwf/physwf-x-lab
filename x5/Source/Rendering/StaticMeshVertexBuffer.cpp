#include "Rendering/StaticMeshVertexBuffer.h"

FStaticMeshVertexBuffer::FStaticMeshVertexBuffer()
{

}

FStaticMeshVertexBuffer::~FStaticMeshVertexBuffer()
{
	CleanUp();
}

void FStaticMeshVertexBuffer::CleanUp()
{

}

void FStaticMeshVertexBuffer::Init(uint32 InNumVertices, uint32 InNumTexCoords, bool bNeedsCPUAccess /*= true*/)
{
	NumTexCoords = InNumTexCoords;
	NumVertices = InNumVertices;

	// Allocate the vertex data storage type.
	AllocateData(bNeedsCPUAccess);
}

void FStaticMeshVertexBuffer::Init(const std::vector<FStaticMeshBuildVertex>& InVertices, uint32 InNumTexCoords)
{
	Init(InVertices.size(), InNumTexCoords);
}

void FStaticMeshVertexBuffer::Init(const FStaticMeshVertexBuffer& InVertexBuffer)
{

}

void FStaticMeshVertexBuffer::AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend)
{

}

void FStaticMeshVertexBuffer::operator=(const FStaticMeshVertexBuffer &Other)
{

}

void FStaticMeshVertexBuffer::InitRHI()
{

}

void FStaticMeshVertexBuffer::ReleaseRHI()
{

}

void FStaticMeshVertexBuffer::InitResource()
{

}

void FStaticMeshVertexBuffer::ReleaseResource()
{

}

void FStaticMeshVertexBuffer::AllocateData(bool bNeedsCPUAccess /*= true*/)
{

}
