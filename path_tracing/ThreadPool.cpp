#include "ThreadPool.h"
#include "Windows.h"
#include "Log.h"

class JFixedThreadPoolImpl
{
public:
	JFixedThreadPoolImpl(size_t MaxThread):ThreadCount(MaxThread), bExit(FALSE), Tasks(NULL)
	{
		InitializeCriticalSection(&CS);
		InitializeConditionVariable(&CV);
		FinishEvents = new HANDLE[ThreadCount];
		for (size_t i = 0; i < ThreadCount; ++i)
		{
			FinishEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
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

	void AppendTask(std::function<void()>&& Func)
	{
		EnterCriticalSection(&CS);
		Tasks = new Task(std::move(Func), Tasks);
		LeaveCriticalSection(&CS);
	}

	void StartWork()
	{
		for (size_t i = 0; i < ThreadCount; ++i)
		{
			SetEvent(FinishEvents[i]);
		}
		WakeAllConditionVariable(&CV);
		Sleep(10);
	}

	void WaitForFinish()
	{
		WaitForMultipleObjects(ThreadCount, FinishEvents, TRUE, INFINITE);
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
				SetEvent(This->FinishEvents[ThreadIndex]);
				X_LOG("%d fall asleep\n", ThreadIndex);
				SleepConditionVariableCS(&This->CV, &This->CS, INFINITE);
				ResetEvent(This->FinishEvents[ThreadIndex]);
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

	size_t ThreadCount;
	CRITICAL_SECTION CS;
	CONDITION_VARIABLE CV;
	HANDLE *FinishEvents;
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

void JFixedThreadPool::AppendTask(std::function<void()>&& Task)
{
	Impl->AppendTask(std::move(Task));
}

void JFixedThreadPool::StartWork()
{
	Impl->StartWork();
}

void JFixedThreadPool::WaitForFinish()
{
	Impl->WaitForFinish();
}

JFixedThreadPool* GFixThreadPool;

void InitFixedThreadPool()
{
	GFixThreadPool = new JFixedThreadPool(MaxThreadIndex());
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
	//return 1;
	return SysInfo.dwNumberOfProcessors;
}

__declspec(thread) int ThreadIndex;
