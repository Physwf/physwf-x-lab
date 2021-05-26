#include "ParallelFor.h"
#include "ThreadPool.h"

class ParalleForTask1D
{
public:
	ParalleForTask1D(std::function<void(size_t)> InFunc1D, size_t InIndex)
		: Func1D(InFunc1D), Index(InIndex)
	{
	}

	void operator()()
	{
		Func1D(Index);
	}
private:
	std::function<void(size_t)> Func1D;
	size_t Index;
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

void ParallelFor(std::function<void(size_t)> Func, size_t Count)
{
	for (size_t i = 0; i < Count; ++i)
	{
		ParalleForTask1D Task1D(Func, i);
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

