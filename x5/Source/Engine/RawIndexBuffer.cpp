#include "RawIndexBuffer.h"
#include "Math/NumericLimits.h"

void FRawIndexBuffer::InitRHI()
{

}

void FRawIndexBuffer16or32::InitRHI()
{

}

void FRawStaticIndexBuffer::SetIndices(const std::vector<uint32>& InIndices, EIndexBufferStride::Type DesiredStride)
{
	int32 NumIndices = (int32)InIndices.size();
	bool bShouldUse32Bit = false;

	if (DesiredStride == EIndexBufferStride::Force32Bit)
	{
		bShouldUse32Bit = true;
	}
	else if (DesiredStride == EIndexBufferStride::AutoDetect)
	{
		int32 i = 0;
		while (!bShouldUse32Bit && i < NumIndices)
		{
			bShouldUse32Bit = InIndices[i] > MAX_uint16;
			i++;
		}
	}

	int32 IndexStride = bShouldUse32Bit ? sizeof(uint32) : sizeof(uint16);
	IndexStorage.reserve(IndexStride*NumIndices);
	IndexStorage.resize(IndexStride*NumIndices);

	if (bShouldUse32Bit)
	{
		check(IndexStorage.size() == InIndices.size() * sizeof(std::vector<uint32>::size_type));
		memcpy(IndexStorage.data(), InIndices.data(), IndexStorage.size());
		b32Bit = true;
	}
	else
	{
		check(IndexStorage.size() == InIndices.size() * sizeof(uint16));
		uint16* DestIndices16Bit = (uint16*)(IndexStorage.data());
		for (int32 i = 0; i < NumIndices; ++i)
		{
			DestIndices16Bit[i] = InIndices[i];
		}
		b32Bit = false;
	}
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
