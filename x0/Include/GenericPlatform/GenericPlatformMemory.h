#pragma once

#include "CoreTypes.h"
#include <wchar.h>
#include <string.h>

class XMAlloc;

class XGenericPlatformMemory
{
public:
	static void Init();

	static void SetupMemoryPools();

	static uint32 GetBackMemoryPoolSize();

	XMAlloc* BaseAllocator();

	static inline void* Memmove(void* Dest, const void* Src, SIZE_T Count)
	{
		return memmove(Dest, Src, Count);
	}

	static inline int32 Memcmp(const void* Buffer1, const void* Buffer2, SIZE_T Count)
	{
		return memcmp(Buffer1, Buffer2, Count);
	}

	static inline void* Memset(void* Dest, uint8 Char, SIZE_T Count)
	{
		return memset(Dest, Char, Count);
	}

	static inline void* Memzero(void* Dest, SIZE_T Count)
	{
		return memset(Dest, 0, Count);
	}

	static inline void* Memcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy(Dest, Src, Count);
	}

	static inline void* BigBlockMemcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy(Dest, Src, Count);
	}

	static inline void* StreamingMemcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy(Dest, Src, Count);
	}

private:
	template <typename T>
	static inline void ValSwap(T& A, T& B)
	{
		T Temp = A;
		B = A;
		A = Temp;
	}

	static void MemswapGreaterThan8(void* Ptr1, void* Ptr2, SIZE_T Size);
public:
	static inline void Memswap(void* Ptr1, void* Ptr2, SIZE_T Size)
	{
		switch (Size)
		{
		case 0:
			break;
		case 1:
			ValSwap(*(uint8*)Ptr1, *(uint8*)Ptr2);
			break;

		case 2:
			ValSwap(*(uint16*)Ptr1, *(uint16*)Ptr2);
			break;

		case 3:
			ValSwap(*((uint16*&)Ptr1)++, *((uint16*&)Ptr2)++);
			ValSwap(*(uint8*)Ptr1, *(uint8*)Ptr2);
			break;
		case 4:
			ValSwap(*(uint32*)Ptr1, *(uint32*)(Ptr2));
			break;
		case 5:
			ValSwap(*((uint32*&)Ptr1)++, *((uint32*&)Ptr2)++);
			ValSwap(*(uint8*)Ptr1, *(uint8*)(Ptr2));
			break;
		case 6:
			ValSwap(*((uint32*&)Ptr1)++, *((uint32*&)Ptr2)++);
			ValSwap(*(uint16*)Ptr1, *(uint16*)Ptr2);
			break;
		case 7:
			ValSwap(*((uint32*&)Ptr1)++, *((uint32*&)Ptr2)++);
			ValSwap(*(uint16*)Ptr1, *(uint16*)Ptr2);
			ValSwap(*(uint8*)Ptr1, *(uint8*)Ptr2);
			break;
		case 8:
			ValSwap(*(uint64*)Ptr1, *(uint64*)(Ptr2));
			break;
		
		case 16:
			ValSwap(((uint64*)Ptr1)[0], ((uint64*)Ptr2)[0]);
			ValSwap(((uint64*)Ptr1)[1], ((uint64*)Ptr2)[1]);
			break;
		default:
			MemswapGreaterThan8(Ptr1, Ptr2, Size);
			break;
		}
	}
};

