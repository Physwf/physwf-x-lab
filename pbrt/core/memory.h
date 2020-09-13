#pragma once

#include "pbrt.h"
#include <list>
#include <cstddef>

#define ARENA_ALLOC(arena,Type) new ((arena).Alloc(sizeof(Type))) Type
void *AllocAligned(size_t size);
template <typename T>
T* AllocAligned(size_t count)
{
	return (T*)AllocAligned(count * sizeof(T));
}
void FreeAligned(void*);

class MemoryArena
{
public:
	MemoryArena(size_t blockSize = 256*1024) :blockSize(blockSize) {}
	~MemoryArena()
	{
		FreeAligned(currentBlock);
		for (auto& block : usedBlocks) FreeAligned(block.second);
		for (auto& block : availableBlocks) FreeAligned(block.second);
	}

	void* Alloc(size_t nBytes)
	{
		const int align = alignof(std::max_align_t);
		nBytes = (nBytes + align - 1)& ~(align - 1);//���뵽align
		//��ǰblock�ռ䲻��
		if (currentBlockPos + nBytes > currentAllocSize)
		{
			if (currentBlock)
			{
				usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
				currentBlock = nullptr;
				currentAllocSize = 0;
			}

			//��ȡ�µ�block

			//��availableBlocks��ȡ
			for (auto iter = availableBlocks.begin(); iter != availableBlocks.end(); ++iter)
			{
				if (iter->first >= nBytes)
				{
					currentAllocSize = iter->first;
					currentBlock = iter->second;
					availableBlocks.erase(iter);
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

	template<typename T>
	T* Alloc(size_t n = 1, bool runConstructor = true)
	{
		T* ret = (T*)Alloc(n * sizeof(T));
		if (runConstructor)
			for (size_t i = 0; i < n; ++i) new (&ret[i]) T();
		return ret;
	}
	void Reset()
	{
		currentBlockPos = 0;
		availableBlocks.splice(availableBlocks.begin(), usedBlocks);
	}
private:
	MemoryArena(const MemoryArena&) = delete;
	MemoryArena& operator=(const MemoryArena&) = delete;
	const size_t blockSize;
	size_t currentBlockPos = 0, currentAllocSize = 0;
	uint8_t* currentBlock = nullptr;
	std::list<std::pair<size_t, uint8_t*>> usedBlocks, availableBlocks;
};

template <typename T, int logBlockSize>
class BlockedArray {
public:
	// BlockedArray Public Methods
	BlockedArray(int uRes, int vRes, const T *d = nullptr)
		: uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize) {
		int nAlloc = RoundUp(uRes) * RoundUp(vRes);
		data = AllocAligned<T>(nAlloc);
		for (int i = 0; i < nAlloc; ++i) new (&data[i]) T();
		if (d)
			for (int v = 0; v < vRes; ++v)
				for (int u = 0; u < uRes; ++u) (*this)(u, v) = d[v * uRes + u];
	}
	constexpr int BlockSize() const { return 1 << logBlockSize; }
	int RoundUp(int x) const {
		return (x + BlockSize() - 1) & ~(BlockSize() - 1);
	}
	int uSize() const { return uRes; }
	int vSize() const { return vRes; }
	~BlockedArray() {
		for (int i = 0; i < uRes * vRes; ++i) data[i].~T();
		FreeAligned(data);
	}
	int Block(int a) const { return a >> logBlockSize; }
	int Offset(int a) const { return (a & (BlockSize() - 1)); }
	T &operator()(int u, int v) {
		int bu = Block(u), bv = Block(v);
		int ou = Offset(u), ov = Offset(v);
		int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
		offset += BlockSize() * ov + ou;
		return data[offset];
	}
	const T &operator()(int u, int v) const {
		int bu = Block(u), bv = Block(v);
		int ou = Offset(u), ov = Offset(v);
		int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
		offset += BlockSize() * ov + ou;
		return data[offset];
	}
	void GetLinearArray(T *a) const {
		for (int v = 0; v < vRes; ++v)
			for (int u = 0; u < uRes; ++u) *a++ = (*this)(u, v);
	}

private:
	// BlockedArray Private Data
	T * data;
	const int uRes, vRes, uBlocks;
};