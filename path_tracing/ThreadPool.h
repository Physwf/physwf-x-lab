#pragma once
#include <functional>

class JFixedThreadPoolImpl;

class JFixedThreadPool
{
public:
	explicit JFixedThreadPool(size_t MaxThread);
	~JFixedThreadPool();

	void AppendTask(std::function<void()>&& Task);
	void StartWork();
	void WaitForFinish();
private:
	JFixedThreadPoolImpl* Impl;
};

extern JFixedThreadPool* GFixThreadPool;

void InitFixedThreadPool();
void FiniFixedThreadPool();
int	MaxThreadIndex();
extern __declspec(thread) int ThreadIndex;