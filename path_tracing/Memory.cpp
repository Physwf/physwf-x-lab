#include "Memory.h"
#include <memory>

void* AllocAligned(size_t Size)
{
	return _aligned_malloc(Size, L1_CACHE_LINE_SIZE);
}

void FreeAligned(void* ptr)
{
	if (!ptr) return;
	_aligned_free(ptr);
}
