#pragma once

#include "CoreTypes.h"










template <typename ReferencedType>
class TRefCountPtr
{
	typedef ReferencedType* ReferenceType;

public:
	TRefCountPtr() :
		Reference(nullptr)
	{}

	~TRefCountPtr()
	{
		if (Reference)
		{
			Reference->Release();
		}
	}

	TRefCountPtr& operator=(ReferencedType* InReference)
	{
		ReferencedType* OldReference = InReference;
		Reference = InReference;
		if (Reference)
		{
			Reference->AddRef();
		}
		if (OldReference)
		{
			OldReference->Release();
		}
		return *this;
	}

	ReferencedType* operator->() const
	{
		return Reference;
	}

	operator ReferenceType() const
	{
		return Reference;
	}

	ReferencedType** GetInitReference()
	{
		*this = nullptr;
		return &Reference;
	}

	ReferencedType* GetReference()
	{
		return Reference;
	}

	bool IsValid()
	{
		return Reference != nullptr;
	}

	uint32 GetRefCount()
	{
		uint32 Result = 0;
		if (Reference)
		{
			Result = Reference->GetRefCount();
		}
		return Result;
	}
private:
	ReferencedType* Reference;
};