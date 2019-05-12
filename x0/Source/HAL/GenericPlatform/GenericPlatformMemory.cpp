#include "GenericPlatform/GenericPlatformMemory.h"


void XGenericPlatformMemory::Init()
{
	SetupMemoryPools();
}

void XGenericPlatformMemory::SetupMemoryPools()
{
	if (XGenericPlatformMemory::GetBackMemoryPoolSize() > 0)
	{

	}
}

uint32 XGenericPlatformMemory::GetBackMemoryPoolSize()
{
	return 0;
}

void XGenericPlatformMemory::MemswapGreaterThan8(void* Ptr1, void* Ptr2, SIZE_T Size)
{

}

