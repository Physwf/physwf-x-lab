#pragma once

#include "Containers/ContainerAllocationPolices.h"
#include "Containers/RecourceArray.h"

enum EResourceAlignment
{
	VERTEXBUFFER_ALIGNMENT = DEFAULT_ALIGNMENT,
	INDEXBUFFER_ALIGNMENT = DEFAULT_ALIGNMENT
};

template <typename ElementType,uint32 Alignment = DEFAULT_ALIGNMENT>
class TResourceArray
	: public XResourceArrayInterface
	, public TArray<ElementType, TAlignedHeapAllocator<Alignment>>
{
public:
	typedef TArray<ElementType, TAlignedHeapAllocator<Alignment>> Super;

	TResourceArray(bool InbNeedsCPUAccess = false)
		:Super(),
		bNeedsCPUAccess(InbNeedsCPUAccess)
	{

	}

	TResourceArray(TResourceArray&&) = default;
	TResourceArray(const TResourceArray&) = default;
	TResourceArray& operator=(TResourceArray&&) = default;
	TResourceArray& operator=(const TResourceArray&) = default;

	virtual ~TResourceArray() = default;

	virtual const void* GetResourceData() const
	{

	}

	virtual uint32 GetResourceDataSize() const
	{

	}

	virtual void Descard()
	{

	}

	virtual bool IsStatic() const
	{
		return false;
	}

	virtual bool GetAllowCPUAccess() const
	{
		return bNeedsCPUAccess;
	}

	virtual void SetAllowCPUAccess(bool bInNeedsCPUAccess)
	{
		bNeedsCPUAccess = bInNeedsCPUAccess;
	}

	TResourceArray& operator=(const Super& Other)
	{
		Super::operator=(Other);
		return *this;
	}


private:
	bool bNeedsCPUAccess;
};