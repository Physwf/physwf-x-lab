#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RenderResource.h"
#include "Containers/DyamiacRHIResourceArray.h"

class XRawIndexBuffer : public XRenderResource
{
public:
	TArray<uint16> Indices;

	void CacheOptimize();

	virtual void InitRHI() override;

};

class XIndexArrayView
{
public:
	XIndexArrayView()
		:UntypedIndexData(NULL),
		, NumInidices(0)
		, b32Bit(false)
	{
	}

	XIndexArrayView(const void* InIndexData,int32 InNumIndices, bool bIn32Bit)
		:UntypedIndexData(InIndexData)
		, NumInidices(InNumIndices),
		, b32Bit(bIn32Bit)
	{
	}

	uint32 operator[](int32 i) { return (uint32)(b32Bit ? ((const uint32*)UntypedIndexData)[i] : ((const uint16*)UntypedIndexData)[i]); }
	uint32 operator[](int32 i) const { return (uint32)(b32Bit ? ((const uint32*)UntypedIndexData)[i] : ((const uint16*)UntypedIndexData)[i]); }
	int32 Num() const { return NumInidices; }
private:
	const void* UntypedIndexData;
	int32 NumInidices;
	bool b32Bit;
};
namespace EIndexBufferStride
{
	enum Type
	{
		Force16Bit = 1,
		Force32Bit = 2,
		AutoDetect = 3,
	};
}
class XRawStaticIndexBuffer : public XIndexBuffer
{
public:
	XRawStaticIndexBuffer(bool bInNeedCPUAccess = false);

	inline void SetIndex(const uint32 At, const uint32 NewIndexValue)
	{

	}

	void SetIndices(const TArray<uint32>& Indices, EIndexBufferStride::Type DisiredStride);

	void InsertIndices(const uint32 At, const uint32* IndicesToAppend, const NumIndicesToAppend);

	void AppendIndices(const uint32* IndicesToAppend, const uint32 NumIndicesToAppend);

	inline uint32 GetIndice(const uint32 At) const
	{

	}

	void RemoveIndicesAt(const uint32 At, const uint32 NumIndicesToRemove);

	void GetCopy(TArray<uint32>& OutIndices) const;

	const uint16* AccessStream16() const;

	XIndexArrayView GetArrayView() const;

	int32 GetNumIndices() const
	{
		return b32Bit ? (IndexStorage.Num() / 4) : (IndexStorage.Num() / 2);
	}

	uint32 GetAllocatedSize() const
	{
		return IndexStorage.GetAllocatedSize();
	}

	void Discard();

	virtual void InitRHI() override;

	inline bool Is32Bit() const { return b32Bit; }
private:
	TResourceArray<uint8, INDEXBUFFER_ALIGNMENT> IndexStorage;
	bool b32Bit;
};