#pragma once
#include <functional>

class JFixedThreadPoolImpl;

class JFixedThreadPool
{
public:
	explicit JFixedThreadPool(size_t MaxThread);
	~JFixedThreadPool();

	void Execute(std::function<void()>&& Task);
private:
	JFixedThreadPoolImpl* Impl;
};

extern JFixedThreadPool* GFixThreadPool;

void InitFixedThreadPool();
void FiniFixedThreadPool();
int	MaxThreadIndex();
extern __declspec(thread) int ThreadIndex;