
#include "parallel.h"
#include "memory.h"
//#include "stats.h"
#include <list>
#include <thread>
#include <condition_variable>
#include <chrono>

static std::vector<std::thread> threads;
static bool shutdownThreads = false;
class ParallelForLoop;
static ParallelForLoop* workList = nullptr;
static std::mutex workListMutex;


class ParallelForLoop
{
public:
	ParallelForLoop(std::function<void(int64_t)> fun1D, int64_t maxIndex, int chunkSize, uint64_t profilerState)
		:func1D(std::move(fun1D)),
		maxIndex(maxIndex),
		chunkSize(chunkSize),
		profilerState(profilerState) {}

	ParallelForLoop(const std::function<void(Point2i)> &f, const Point2i& count, uint64_t profilerState)
		:func2D(f),
		maxIndex(count.x*count.y),
		chunkSize(1),
		profilerState(profilerState)
	{
		nX = count.x;
	}
public:
	std::function<void(int64_t)> func1D;
	std::function<void(Point2i)> func2D;
	const int64_t maxIndex;
	const int chunkSize;
	uint64_t profilerState;
	int64_t nextIndex = 0;
	int activeWorker = 0;
	ParallelForLoop* next = nullptr;
	int nX = -1;

	bool Finished() const
	{
		return nextIndex >= maxIndex && activeWorker == 0;
	}
};

void Barrier::Wait()
{
	std::unique_lock<std::mutex> lock(mutex);
	DOCHECK(count > 0);
	if (--count == 0)
	{
		cv.notify_all();
	}
	else
	{
		cv.wait(lock, [this] { return count == 0; });
	}
}

static std::condition_variable workListCondition;

static void workerThreadFunc(int tIndex, std::shared_ptr<Barrier> barrier)
{
	ThreadIndex = tIndex;

	barrier->Wait();

	barrier.reset();

	std::unique_lock<std::mutex> lock(workListMutex);
	while (!shutdownThreads)
	{
		if (!workList)
		{
			workListCondition.wait(lock);
		}
		else
		{
			ParallelForLoop &loop = *workList;

			int64_t indexStart = loop.nextIndex;
			int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);
			loop.nextIndex = indexEnd;
			//printf("ThreadIndex=%d loop.nextIndex==%I64d\n", ThreadIndex, loop.nextIndex);
			if (loop.nextIndex == loop.maxIndex) workList = loop.next;
			loop.activeWorker++;
			//printf("loop.activeWorker++=%d\n", loop.activeWorker);
			lock.unlock();
			for (int64_t index = indexStart; index < indexEnd; ++index)
			{
				if (loop.func1D)
				{
					loop.func1D(index);
				}
				else
				{
/*					std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();*/
					loop.func2D(Point2i(index%loop.nX, index / loop.nX));
// 					std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
// 					int64_t elapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
// 					printf("elapsedMS:%lld\n", elapsedMS);
				}
			}
			lock.lock();

			loop.activeWorker--;
			//printf("ThreadIndex=%d loop.activeWorker--=%d\n", ThreadIndex, loop.activeWorker);
			if (loop.Finished()) workListCondition.notify_all();
		}
	}
}

void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize /*= 1*/)
{
	if (threads.empty() || count < chunkSize) 
	{
		for (int64_t i = 0; i < count; ++i) func(i);
		return;
	}

	ParallelForLoop loop(std::move(func), count, chunkSize, 0);

	workListMutex.lock();
	loop.next = workList;
	workList = &loop;
	workListMutex.unlock();

	std::unique_lock<std::mutex> lock(workListMutex);
	workListCondition.notify_all();

	while (!loop.Finished())
	{
		int64_t indexStart = loop.nextIndex;
		int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

		loop.nextIndex = indexEnd;
		if (loop.nextIndex == loop.maxIndex) workList = loop.next;
		loop.activeWorker++;

		lock.unlock();
		for (int64_t index = indexStart; index < indexEnd; ++index) {
			//uint64_t oldState = ProfilerState;
			//ProfilerState = loop.profilerState;
			if (loop.func1D) 
			{
				loop.func1D(index);
			}
			// Handle other types of loops
			else 
			{
				//CHECK(loop.func2D);
				loop.func2D(Point2i(index % loop.nX, index / loop.nX));
			}
			//ProfilerState = oldState;
		}
		lock.lock();

		// Update _loop_ to reflect completion of iterations
		loop.activeWorker--;
	}
}

void ParallelFor2D(std::function<void(Point2i)> func, const Point2i& count)
{
	if (threads.empty() || count.x *count.y <= 1)
	{
		for (int y = 0; y < count.y; ++y)
		{
			for (int x = 0; x < count.x; ++x)
			{
				func(Point2i(x,y));
			}
		}
	}

	ParallelForLoop loop(std::move(func), count, 0);

	{
		std::lock_guard<std::mutex> lock(workListMutex);
		loop.next = workList;
		workList = &loop;
	}

	std::unique_lock<std::mutex> lock(workListMutex);
	workListCondition.notify_all();

	while (!loop.Finished()) {
		// Run a chunk of loop iterations for _loop_

		// Find the set of loop iterations to run next
		int64_t indexStart = loop.nextIndex;
		int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

		// Update _loop_ to reflect iterations this thread will run
		loop.nextIndex = indexEnd;
		//printf("loop.nextIndex==%I64d\n", loop.nextIndex);
		if (loop.nextIndex == loop.maxIndex) workList = loop.next;
		loop.activeWorker++;
		//printf("loop.activeWorker++=%d\n", loop.activeWorker);
		// Run loop indices in _[indexStart, indexEnd)_
		lock.unlock();
		for (int64_t index = indexStart; index < indexEnd; ++index) {
			//uint64_t oldState = ProfilerState;
			//ProfilerState = loop.profilerState;
			if (loop.func1D) 
			{
				loop.func1D(index);
			}
			// Handle other types of loops
			else 
			{
				//CHECK(loop.func2D);
				loop.func2D(Point2i(index % loop.nX, index / loop.nX));
			}
			//ProfilerState = oldState;
		}
		lock.lock();
		
		// Update _loop_ to reflect completion of iterations
		loop.activeWorker--;
		//printf("loop.activeWorker--=%d\n", loop.activeWorker);
	}
}

thread_local int ThreadIndex;

int MaxThreadIndex()
{
	return PbrtOptions.nThreads == 0 ? NumSystemCores() : PbrtOptions.nThreads;
}

int NumSystemCores()
{
	return std::max(1u, std::thread::hardware_concurrency());
}

void ParallelInit()
{
	int nThreads = MaxThreadIndex();
	ThreadIndex = 0;
	std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(nThreads);
	for (int i = 0; i < nThreads - 1; ++i)
	{
		threads.push_back(std::thread(workerThreadFunc, i + 1, barrier));
	}

	barrier->Wait();
}

void ParallelCleanup()
{
	if (threads.empty()) return;
	
	{
		std::lock_guard<std::mutex> lock(workListMutex);
		shutdownThreads = true;
		workListCondition.notify_all();
	}

	for (std::thread& thread : threads) thread.join();
	threads.erase(threads.begin(), threads.end());
	shutdownThreads = false;
}

void MergedWorkerThreadStats()
{

}
