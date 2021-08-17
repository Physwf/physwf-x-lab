#include "ParallelFor.h"
#include "ThreadPool.h"

class ParalleForTask1D
{
public:
	ParalleForTask1D(std::function<void(size_t)> InFunc1D, size_t InMaxIndex, size_t InChunkSize)
		: Func1D(InFunc1D), MaxIndex(InMaxIndex), ChunckSize(InChunkSize)
	{
	}

	void operator()()
	{
		size_t IndexStart = NextIndex;
		size_t IndexEnd = std::min(IndexStart + ChunckSize, MaxIndex);
		for (size_t i = IndexStart; i < IndexEnd; ++i)
		{
			Func1D(i);
		}
	}
	size_t NextIndex = 0;

private:
	std::function<void(size_t)> Func1D;
	size_t MaxIndex;
	size_t ChunckSize;
};

class ParalleForTask2D
{
public:
	ParalleForTask2D(std::function<void(Vector2i)> InFunc2D, Vector2i InIndex)
		: Func2D(InFunc2D), Index(InIndex)
	{
	}

	void operator()()
	{
		Func2D(Index);
	}
private:
	std::function<void(Vector2i)> Func2D;
	Vector2i Index;
};

void ParallelFor(std::function<void(size_t)> Func, size_t Count,size_t chunkSize)
{
	for (int i = 0; i < Count; i += chunkSize)
	{
		ParalleForTask1D Task1D(Func, Count, chunkSize);
		Task1D.NextIndex = i;
		if (GFixThreadPool != NULL)
		{
			GFixThreadPool->Execute(Task1D);
		}
	}
}

void ParallelFor2D(std::function<void(Vector2i)> Func, Vector2i Count)
{
	for (size_t y = 0; y < Count.Y; ++y)
	{
		for (size_t x = 0; x < Count.X; ++x)
		{
			ParalleForTask2D Task2D(Func,Vector2i(x,y));
			if (GFixThreadPool != NULL)
			{
				GFixThreadPool->Execute(Task2D);
			}
		}
	}
}

