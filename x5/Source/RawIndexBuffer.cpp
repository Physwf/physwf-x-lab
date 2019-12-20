#include "RawIndexBuffer.h"

void FRawIndexBuffer::InitRHI()
{

}

void FRawIndexBuffer16or32::InitRHI()
{

}

void FRawStaticIndexBuffer::SetIndices(const std::vector<uint32>& InIndices, EIndexBufferStride::Type DesiredStride)
{

}

void FRawStaticIndexBuffer::InsertIndices(const uint32 At, const uint32* IndicesToAppend, const uint32 NumIndicesToAppend)
{

}

void FRawStaticIndexBuffer::AppendIndices(const uint32* IndicesToAppend, const uint32 NumIndicesToAppend)
{

}

void FRawStaticIndexBuffer::RemoveIndicesAt(const uint32 At, const uint32 NumIndicesToRemove)
{

}

void FRawStaticIndexBuffer::GetCopy(std::vector<uint32>& OutIndices) const
{

}

const uint16* FRawStaticIndexBuffer::AccessStream16() const
{
	return nullptr;
}

void FRawStaticIndexBuffer::Discard()
{

}

void FRawStaticIndexBuffer::InitRHI()
{

}
