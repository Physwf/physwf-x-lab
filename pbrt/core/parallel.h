#pragma once

#include "pbrt.h"
#include "geometry.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class AtomicFloat
{
public:
	explicit AtomicFloat(Float v = 0) { bits = FloatToBits(v); }
	operator Float() const { return BitsToFloat(bits); }

	Float operator=(Float v)
	{
		bits = FloatToBits(v);
		return v;
	}

	void Add(Float v)
	{
#ifdef PBRT_FLOAT_AS_DOUBLE
		uint64_t oldBits = bits, newBits;
#else
		uint32_t oldBits = bits, newBits;
#endif
		do 
		{
			newBits = FloatToBits(BitsToFloat(oldBits) + v);
		} while (!bits.compare_exchange_weak(oldBits,newBits));
	}

private:
	// AtomicFloat Private Data
#ifdef PBRT_FLOAT_AS_DOUBLE
	std::atomic<uint64_t> bits;
#else
	std::atomic<uint32_t> bits;
#endif
};

class Barrier
{
public:
	Barrier(int count) :count(count) { DOCHECK(count>0); }
	~Barrier() { DOCHECK(count == 0); }
	void Wait();
private:
	std::mutex mutex;
	std::condition_variable cv;
	int count;
};

void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chuckSize = 1);
extern thread_local int ThreadIndex;
void ParallelFor2D(std::function<void(Point2i)> func, const Point2i& count);
int MaxThreadIndex();
int NumSystemCores();

void ParallelInit();
void ParallelCleanup();
void MergedWorkerThreadStats();
