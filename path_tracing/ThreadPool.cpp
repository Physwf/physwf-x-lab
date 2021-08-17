#include "ThreadPool.h"
#include "Windows.h"

class JFixedThreadPoolImpl
{
public:
	JFixedThreadPoolImpl(size_t MaxThread):bExit(FALSE), Tasks(NULL)
	{
		InitializeCriticalSection(&CS);
		InitializeConditionVariable(&CV);
		FinishEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		for (size_t i = 0; i < MaxThread; ++i)
		{
			CreateThread(NULL, 0, ThreadRun, this, 0, NULL);
		}
	}
	~JFixedThreadPoolImpl()
	{
		EnterCriticalSection(&CS);
		bExit = TRUE;
		LeaveCriticalSection(&CS);

		WakeAllConditionVariable(&CV);
	}

	void Execute(std::function<void()>&& Func)
	{
		EnterCriticalSection(&CS);
		Tasks = new Task(std::move(Func), Tasks);
		LeaveCriticalSection(&CS);
		ResetEvent(FinishEvent);
		WakeConditionVariable(&CV);
		WaitForSingleObject(FinishEvent, INFINITE);
	}
private:
	static DWORD WINAPI ThreadRun(LPVOID Param)
	{
		static int index = 0;
		JFixedThreadPoolImpl* This = (JFixedThreadPoolImpl*)Param;
		EnterCriticalSection(&This->CS);
		ThreadIndex = index++;
		while (true)
		{
			if (This->Tasks != NULL)
			{
				Task* Current = This->Tasks;
				This->Tasks = This->Tasks->Next;
				LeaveCriticalSection(&This->CS);
				Current->Func();
				//delete Current;
				EnterCriticalSection(&This->CS);
			}
			else if (This->bExit)
			{
				break;
			}
			else
			{
				SetEvent(This->FinishEvent);
				SleepConditionVariableCS(&This->CV, &This->CS, INFINITE);
			}
		}
		LeaveCriticalSection(&This->CS);
		return 0;
	}
private:
	struct Task
	{
		Task(std::function<void()>&& InFunc, Task* InNext) : Func(InFunc), Next(InNext)
		{
		}
		std::function<void()> Func;
		Task* Next;
	};

	CRITICAL_SECTION CS;
	CONDITION_VARIABLE CV;
	HANDLE FinishEvent;
	BOOL bExit;
	Task* Tasks;
};



JFixedThreadPool::JFixedThreadPool(size_t MaxThread)
	: Impl(new JFixedThreadPoolImpl(MaxThread))
{

}

JFixedThreadPool::~JFixedThreadPool()
{
	delete Impl;
}

void JFixedThreadPool::Execute(std::function<void()>&& Task)
{
	Impl->Execute(std::move(Task));
}

JFixedThreadPool* GFixThreadPool;

void InitFixedThreadPool()
{
	//GFixThreadPool = new JFixedThreadPool(MaxThreadIndex);
	GFixThreadPool = new JFixedThreadPool(1);
}

void FiniFixedThreadPool()
{
	if (GFixThreadPool != NULL)
	{
		delete GFixThreadPool;
	}
}

int MaxThreadIndex()
{
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	return 1;
	return SysInfo.dwNumberOfProcessors;
}

__declspec(thread) int ThreadIndex;
