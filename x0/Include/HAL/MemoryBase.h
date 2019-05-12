#pragma once

#include "X0.h"
#include "CoreTypes.h"

enum
{
	DEFAULT_ALIGNMENT	= 0,
	MIN_ALIGNMENT		= 8,
};

class XUseSystemMallocForNew
{
	void* operator	new (size_t Size);
	void operator	delete(void* Ptr);

	void* operator	new[](size_t Size);
	void operator	delete[](void* Ptr);
};

class X0_API XMAlloc : public XUseSystemMallocForNew
{
	virtual void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT) = 0;
	virtual void* Realloc(SIZE_T Count, uint32 Aligment = DEFAULT_ALIGNMENT) = 0;
	virtual void Free(void* Orignal) = 0;

};