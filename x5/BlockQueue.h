#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

namespace Net
{
	template<typename T>
	class BlockQueue
	{
	public:
		BlockQueue() {}
		~BlockQueue() {}

		void Push(const T& value)
		{
			std::unique_lock lk(mMutex);
			mQueue.push_back(value);
			mCV.notify_one();
		}

		T Pop()
		{
			std::unique_lock lk(mMutex);
			if (mQueue.size() == 0)
			{
				mCV.wait(lk);
			}
			T value = mQueue.front();
			mQueue.pop_front();
			return value;
		}

		void Clear()
		{
			std::unique_lock lk(mMutex);
			mQueue.clear();
		}

	private:
		std::condition_variable mCV;
		std::mutex mMutex;
		std::deque<T> mQueue;
	};
}