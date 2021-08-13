#pragma once

#include <list>
#include <algorithm>

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

template<typename T,int logBlockSize/*BlockSize的log值*/>
class BlockArray2D
{
public:
	BlockArray2D(int u, int v, const T* d = nullptr)
		: uDim(u), vDim(v),uBlocks(RoundUp(uDim) >> logBlockSize)
	{
		int nAlloc = RoundUp(uDim) * RoundUp(vDim);//
		data = AllocAligned<T>(nAlloc);
		for (int i = 0; i < nAlloc; ++i) new (&data[i])T();
		if (d)
			for (int v = 0; v < vDim; ++v)
				for (int u = 0; u < uDim; ++u)
					(*this)(u, v) = d[v * uDim + u];
	}
	~BlockArray2D()
	{
		for (int i = 0; i < uDim * vDim; ++i) data[i].~T();
		FreeAligned(data);
	}
	int uSize() const { return uDim; }
	int vSize() const { return vDim; }
	constexpr int BlockSize() const { return 1 << logBlockSize; }
	int RoundUp(int x) const//补足使返回值能被blocksize整除
	{
		return (x + BlockSize() - 1) & ~(BlockSize() - 1);
		//								使尾数为0
	}
	int Block(int a) const { return a >> logBlockSize; }
	int Offset(int a) const { return (a & (BlockSize() - 1)); }
	T& operator()(int u, int v)
	{
		int bu = Block(u), bv = Block(v);
		int ou = Offset(u), ov = Offset(v);
		int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
		offset += BlockSize() * ov + ou;
		return data[offset];
	}
	const T& operator(int u, int v) const
	{
		int bu = Block(u), bv = Block(v);
		int ou = Offset(u), ov = Offset(v);
		int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
		offset += BlockSize() * ov + ou;
		return data[offset];
	}
private:
	T* data;
	const int uDim, vDim;//数组的两个维度大小
	const int uBlocks;//u方向的block数量
};