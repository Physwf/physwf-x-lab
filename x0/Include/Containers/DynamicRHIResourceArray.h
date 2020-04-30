#pragma once

#include "CoreTypes.h"
#include "Templates/UnrealTypeTraits.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Containers/Array.h"
#include "CoreGlobals.h"
#include "Containers/ResourceArray.h"

/** alignment for supported resource types */
enum EResourceAlignment
{
	VERTEXBUFFER_ALIGNMENT = DEFAULT_ALIGNMENT,
	INDEXBUFFER_ALIGNMENT = DEFAULT_ALIGNMENT
};

template< typename ElementType, uint32 Alignment = DEFAULT_ALIGNMENT >
class TResourceArray
	: public FResourceArrayInterface
	, public TArray<ElementType, TAlignedHeapAllocator<Alignment> >
{
public:
	typedef TArray<ElementType, TAlignedHeapAllocator<Alignment> > Super;

	TResourceArray(bool InNeedsCPUAccess = false)
		: Super()
		, bNeedsCPUAccess(InNeedsCPUAccess)
	{
	}

	TResourceArray(TResourceArray&&) = default;
	TResourceArray(const TResourceArray&) = default;
	TResourceArray& operator=(TResourceArray&&) = default;
	TResourceArray& operator=(const TResourceArray&) = default;

	virtual ~TResourceArray() = default;


	// FResourceArrayInterface

	/**
	* @return A pointer to the resource data.
	*/
	virtual const void* GetResourceData() const
	{
		return &(*this)[0];
	}

	/**
	* @return size of resource data allocation
	*/
	virtual uint32 GetResourceDataSize() const
	{
		return this->Num() * sizeof(ElementType);
	}

	/**
	* Called on non-UMA systems after the RHI has copied the resource data, and no longer needs the CPU's copy.
	* Only discard the resource memory on clients, and if the CPU doesn't need access to it.
	* Non-clients can't discard the data because they may need to serialize it.
	*/
	virtual void Discard()
	{
		if (!bNeedsCPUAccess/* && FPlatformProperties::RequiresCookedData() && !IsRunningCommandlet()*/)
		{
			this->Empty();
		}
	}

	/**
	* @return true if the resource array is static and shouldn't be modified
	*/
	virtual bool IsStatic() const
	{
		return false;
	}

	/**
	* @return true if the resource keeps a copy of its resource data after the RHI resource has been created
	*/
	virtual bool GetAllowCPUAccess() const
	{
		return bNeedsCPUAccess;
	}

	/**
	* Sets whether the resource array will be accessed by CPU.
	*/
	virtual void SetAllowCPUAccess(bool bInNeedsCPUAccess)
	{
		bNeedsCPUAccess = bInNeedsCPUAccess;
	}

	// Assignment operators.
	TResourceArray& operator=(const Super& Other)
	{
		Super::operator=(Other);
		return *this;
	}

private:
	/**
	* true if this array needs to be accessed by the CPU.
	* If no CPU access is needed then the resource is freed once its RHI resource has been created
	*/
	bool bNeedsCPUAccess;
};
