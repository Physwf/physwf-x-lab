#include "Rendering/ColorVertexBuffer.h"

FColorVertexBuffer::FColorVertexBuffer()
{

}

FColorVertexBuffer::~FColorVertexBuffer()
{

}

void FColorVertexBuffer::CleanUp()
{

}

void FColorVertexBuffer::Init(uint32 InNumVertices)
{

}

void FColorVertexBuffer::Init(const TArray<FStaticMeshBuildVertex>& InVertices)
{

}

void FColorVertexBuffer::Init(const FColorVertexBuffer& InVertexBuffer)
{

}

void FColorVertexBuffer::AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend)
{

}

uint32 FColorVertexBuffer::GetAllocatedSize() const
{

}

void FColorVertexBuffer::GetVertexColors(std::vector<FColor>& OutColors) const
{

}

void FColorVertexBuffer::InitFromColorArray(const FColor *InColors, uint32 Count, uint32 Stride /*= sizeof(FColor)*/)
{

}

void FColorVertexBuffer::InitRHI()
{

}

void FColorVertexBuffer::ReleaseRHI()
{

}

void FColorVertexBuffer::operator=(const FColorVertexBuffer &Other)
{

}

