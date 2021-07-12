#pragma once

#include <list>

#define L1_CACHE_LINE_SIZE 64

#define ARENA_ALLOC(arena, Type) new ((arena).Alloc(sizeof(Type))) Type

void* AllocAligned(size_t size);

template<typename T> 
T* AllocAligned(size_t count) 
{ 
	return (T*)AllocAligned(count * sizeof(T));
}
void FreeAligned(void*);

class alignas(L1_CACHE_LINE_SIZE) MemoryArena
{
public:
	MemoryArena(size_t blockSize = 256 * 1024) : blockSize(blockSize) {}
	~MemoryArena()
	{
		FreeAligned(currentBlock);
		for (auto& block : usedBlocks) FreeAligned(block.second);
		for (auto& block : availableBlocks) FreeAligned(block.second);
	}

	void* Alloc(size_t nBytes)
	{
		const int align = alignof(std::max_align_t);
		nBytes = (nBytes + align - 1) & ~(align - 1);
		if (currentBlockPos + nBytes > currentAllocSize)
		{
			if (currentBlock)
			{
				usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
				currentBlock = nullptr;
				currentAllocSize = 0;
			}

			for (auto it = availableBlocks.begin(); it != availableBlocks.end(); ++it)
			{
				if (it->first >= nBytes)
				{
					currentAllocSize = it->first;
					currentBlock = it->second;
					availableBlocks.erase(it);
					break;
				}
			}

			if (!currentBlock)
			{
				currentAllocSize = std::max(nBytes, blockSize);
				currentBlock = AllocAligned<uint8_t>(currentAllocSize);
			}
			currentBlockPos = 0;
		}

		void* ret = currentBlock + currentBlockPos;
		currentBlockPos += nBytes;
		return ret;
	}

	template <typename T>
	T* Alloc(size_t n = 1, bool runConstructor = true)
	{
		T* ret = (T*)Alloc(n * sizeof(T));
		if (runConstructor)
			for (size_t i = 0; i < n; ++i) new (&ret[i]) T();
		return ret;
	}

	size_t Reset()
	{
		size_t total = currentAllocSize;
		for (const auto& alloc : usedBlocks) total += alloc.first;
		for (const auto& alloc : availableBlocks) total += alloc.first;
		return total;
	}
private:
	MemoryArena(const MemoryArena&) = delete;
	MemoryArena& operator=(const MemoryArena&) = delete;
private:
	const size_t blockSize;
	size_t currentBlockPos = 0;
	size_t currentAllocSize = 0;
	uint8_t* currentBlock = nullptr;
	std::list<std::pair<size_t, uint8_t*>>  usedBlocks;
	std::list<std::pair<size_t, uint8_t*>>  availableBlocks;
};