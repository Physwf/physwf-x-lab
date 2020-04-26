#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Templates/Function.h"
//#include "Delegates/Delegate.h"
#include "HAL/ThreadSafeCounter.h"
#include "Containers/LockFreeList.h"
//#include "Stats/Stats.h"
//#include "HAL/IConsoleManager.h"
#include "HAL/Event.h"
#include "Templates/RefCounting.h"
#include "Containers/LockFreeFixedSizeAllocator.h"
#include "Misc/MemStack.h"
#include "Templates/Atomic.h"

#include "X0.h"

// what level of checking to perform...normally checkSlow but could be ensure or check
#define checkThreadGraph checkSlow

class FGraphEvent;

namespace ENamedThreads
{
	enum Type
	{
		UnusedAnchor = -1,
		/** The always-present, named threads are listed next **/
#if STATS
		StatsThread,
#endif
		RHIThread,
		AudioThread,
		GameThread,
		// The render thread is sometimes the game thread and is sometimes the actual rendering thread
		ActualRenderingThread = GameThread + 1,
		// CAUTION ThreadedRenderingThread must be the last named thread, insert new named threads before it

		/** not actually a thread index. Means "Unknown Thread" or "Any Unnamed Thread" **/
		AnyThread = 0xff,

		/** High bits are used for a queue index and priority**/

		MainQueue = 0x000,
		LocalQueue = 0x100,

		NumQueues = 2,
		ThreadIndexMask = 0xff,
		QueueIndexMask = 0x100,
		QueueIndexShift = 8,

		/** High bits are used for a queue index task priority and thread priority**/

		NormalTaskPriority = 0x000,
		HighTaskPriority = 0x200,

		NumTaskPriorities = 2,
		TaskPriorityMask = 0x200,
		TaskPriorityShift = 9,

		NormalThreadPriority = 0x000,
		HighThreadPriority = 0x400,
		BackgroundThreadPriority = 0x800,

		NumThreadPriorities = 3,
		ThreadPriorityMask = 0xC00,
		ThreadPriorityShift = 10,

		/** Combinations **/
#if STATS
		StatsThread_Local = StatsThread | LocalQueue,
#endif
		GameThread_Local = GameThread | LocalQueue,
		ActualRenderingThread_Local = ActualRenderingThread | LocalQueue,

		AnyHiPriThreadNormalTask = AnyThread | HighThreadPriority | NormalTaskPriority,
		AnyHiPriThreadHiPriTask = AnyThread | HighThreadPriority | HighTaskPriority,

		AnyNormalThreadNormalTask = AnyThread | NormalThreadPriority | NormalTaskPriority,
		AnyNormalThreadHiPriTask = AnyThread | NormalThreadPriority | HighTaskPriority,

		AnyBackgroundThreadNormalTask = AnyThread | BackgroundThreadPriority | NormalTaskPriority,
		AnyBackgroundHiPriTask = AnyThread | BackgroundThreadPriority | HighTaskPriority,
	};

	struct FRenderThreadStatics
	{
	private:
		// These are private to prevent direct access by anything except the friend functions below
		static X0_API TAtomic<Type> RenderThread;
		static X0_API TAtomic<Type> RenderThread_Local;

		friend Type GetRenderThread();
		friend Type GetRenderThread_Local();
		friend void SetRenderThread(Type Thread);
		friend void SetRenderThread_Local(Type Thread);
	};

	FORCEINLINE Type GetRenderThread()
	{
		return FRenderThreadStatics::RenderThread.Load(EMemoryOrder::Relaxed);
	}

	FORCEINLINE Type GetRenderThread_Local()
	{
		return FRenderThreadStatics::RenderThread_Local.Load(EMemoryOrder::Relaxed);
	}

	FORCEINLINE void SetRenderThread(Type Thread)
	{
		FRenderThreadStatics::RenderThread.Store(Thread, EMemoryOrder::Relaxed);
	}

	FORCEINLINE void SetRenderThread_Local(Type Thread)
	{
		FRenderThreadStatics::RenderThread_Local.Store(Thread, EMemoryOrder::Relaxed);
	}

	// these allow external things to make custom decisions based on what sorts of task threads we are running now.
	// this are bools to allow runtime tuning.
	extern X0_API int32 bHasBackgroundThreads;
	extern X0_API int32 bHasHighPriorityThreads;

	FORCEINLINE Type GetThreadIndex(Type ThreadAndIndex)
	{
		return ((ThreadAndIndex & ThreadIndexMask) == AnyThread) ? AnyThread : Type(ThreadAndIndex & ThreadIndexMask);
	}

	FORCEINLINE int32 GetQueueIndex(Type ThreadAndIndex)
	{
		return (ThreadAndIndex & QueueIndexMask) >> QueueIndexShift;
	}

	FORCEINLINE int32 GetTaskPriority(Type ThreadAndIndex)
	{
		return (ThreadAndIndex & TaskPriorityMask) >> TaskPriorityShift;
	}

	FORCEINLINE int32 GetThreadPriorityIndex(Type ThreadAndIndex)
	{
		int32 Result = (ThreadAndIndex & ThreadPriorityMask) >> ThreadPriorityShift;
		check(Result >= 0 && Result < NumThreadPriorities);
		return Result;
	}

	FORCEINLINE Type SetPriorities(Type ThreadAndIndex, Type ThreadPriority, Type TaskPriority)
	{
		check(
			!(ThreadAndIndex & ~ThreadIndexMask) &&  // not a thread index
			!(ThreadPriority & ~ThreadPriorityMask) && // not a thread priority
			(ThreadPriority & ThreadPriorityMask) != ThreadPriorityMask && // not a valid thread priority
			!(TaskPriority & ~TaskPriorityMask) // not a task priority
		);
		return Type(ThreadAndIndex | ThreadPriority | TaskPriority);
	}

	FORCEINLINE Type SetPriorities(Type ThreadAndIndex, int32 PriorityIndex, bool bHiPri)
	{
		check(
			!(ThreadAndIndex & ~ThreadIndexMask) && // not a thread index
			PriorityIndex >= 0 && PriorityIndex < NumThreadPriorities // not a valid thread priority
		);
		return Type(ThreadAndIndex | (PriorityIndex << ThreadPriorityShift) | (bHiPri ? HighTaskPriority : NormalTaskPriority));
	}

	FORCEINLINE Type SetThreadPriority(Type ThreadAndIndex, Type ThreadPriority)
	{
		check(
			!(ThreadAndIndex & ~ThreadIndexMask) &&  // not a thread index
			!(ThreadPriority & ~ThreadPriorityMask) && // not a thread priority
			(ThreadPriority & ThreadPriorityMask) != ThreadPriorityMask // not a valid thread priority
		);
		return Type(ThreadAndIndex | ThreadPriority);
	}

	FORCEINLINE Type SetTaskPriority(Type ThreadAndIndex, Type TaskPriority)
	{
		check(
			!(ThreadAndIndex & ~ThreadIndexMask) &&  // not a thread index
			!(TaskPriority & ~TaskPriorityMask) // not a task priority
		);
		return Type(ThreadAndIndex | TaskPriority);
	}

}


namespace ESubsequentsMode
{
	enum Type
	{
		/** Necessary when another task will depend on this task. */
		TrackSubsequents,
		/** Can be used to save task graph overhead when firing off a task that will not be a dependency of other tasks. */
		FireAndForget
	};
}

/** Convenience typedef for a reference counted pointer to a graph event **/
typedef TRefCountPtr<class FGraphEvent> FGraphEventRef;

/** Convenience typedef for a an array a graph events **/
typedef TArray<FGraphEventRef, TInlineAllocator<4> > FGraphEventArray;

class FTaskGraphInterface
{
	friend class FBaseGraphTask;
	/**
	 *	Internal function to queue a task
	 *	@param	Task; the task to queue
	 *	@param	ThreadToExecuteOn; Either a named thread for a threadlocked task or ENamedThreads::AnyThread for a task that is to run on a worker thread
	 *	@param	CurrentThreadIfKnown; This should be the current thread if it is known, or otherwise use ENamedThreads::AnyThread and the current thread will be determined.
	**/
	virtual void QueueTask(class FBaseGraphTask* Task, ENamedThreads::Type ThreadToExecuteOn, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread) = 0;

public:

	virtual ~FTaskGraphInterface()
	{
	}

	// Startup, shutdown and singleton API

	/**
	 *	Explicit start call for the system. The ordinary singleton pattern does not work because internal threads start asking for the singleton before the constructor has returned.
	 *	@param	NumThreads; Total number of threads in the system, must be 0 to disable separate taskgraph thread, at least 2 if !threadedrendering, else at least 3
	**/
	static X0_API void Startup(int32 NumThreads);
	/**
	 *	Explicit start call to shutdown the system. This is unlikely to work unless the system is idle.
	**/
	static X0_API void Shutdown();
	/**
	 *	Check to see if the system is running.
	 **/
	static X0_API bool IsRunning();
	/**
	 *	Singleton for the system
	 *	@return a reference to the task graph system
	**/
	static X0_API FTaskGraphInterface& Get();

	/** Return the current thread type, if known. **/
	virtual ENamedThreads::Type GetCurrentThreadIfKnown(bool bLocalQueue = false) = 0;

	/**
		Return the number of worker (non-named) threads PER PRIORITY SET.
		This is useful for determining how many tasks to split a job into.
	**/
	virtual	int32 GetNumWorkerThreads() = 0;

	/** Return true if the given named thread is processing tasks. This is only a "guess" if you ask for a thread other than yourself because that can change before the function returns. **/
	virtual bool IsThreadProcessingTasks(ENamedThreads::Type ThreadToCheck) = 0;

	// External Thread API

	/**
	 *	A one time call that "introduces" an external thread to the system. Basically, it just sets up the TLS info
	 *	@param	CurrentThread; The name of the current thread.
	**/
	virtual void AttachToThread(ENamedThreads::Type CurrentThread) = 0;

	/**
	 *	Requests that a named thread, which must be this thread, run until idle, then return.
	 *	@param	CurrentThread; The name of this thread
	**/
	virtual void ProcessThreadUntilIdle(ENamedThreads::Type CurrentThread) = 0;

	/**
	 *	Requests that a named thread, which must be this thread, run until an explicit return request is received, then return.
	 *	@param	CurrentThread; The name of this thread
	**/
	virtual void ProcessThreadUntilRequestReturn(ENamedThreads::Type CurrentThread) = 0;

	/**
	 *	Request that the given thread stop when it is idle
	**/
	virtual void RequestReturn(ENamedThreads::Type CurrentThread) = 0;

	/**
	 *	Requests that a named thread, which must be this thread, run until a list of tasks is complete.
	 *	@param	Tasks - tasks to wait for
	 *	@param	CurrentThread - This thread, must be a named thread
	**/
	virtual void WaitUntilTasksComplete(const FGraphEventArray& Tasks, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread) = 0;

	/**
	 *	When a set of tasks complete, fire a scoped event
	 *	@param	InEvent - event to fire when the task is done
	 *	@param	Tasks - tasks to wait for
	 *	@param	CurrentThreadIfKnown - This thread, if known
	**/
	virtual void TriggerEventWhenTasksComplete(FEvent* InEvent, const FGraphEventArray& Tasks, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread) = 0;

	/**
	 *	Requests that a named thread, which must be this thread, run until a task is complete
	 *	@param	Task - task to wait for
	 *	@param	CurrentThread - This thread, must be a named thread
	**/
	void WaitUntilTaskCompletes(const FGraphEventRef& Task, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		FGraphEventArray Prerequistes;
		Prerequistes.Add(Task);
		WaitUntilTasksComplete(Prerequistes, CurrentThreadIfKnown);
	}

	/**
	 *	When a task completes, fire a scoped event
	 *	@param	InEvent - event to fire when the task is done
	 *	@param	Task - task to wait for
	 *	@param	CurrentThreadIfKnown - This thread, if known
	**/
	void TriggerEventWhenTaskCompletes(FEvent* InEvent, const FGraphEventRef& Task, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		FGraphEventArray Prerequistes;
		Prerequistes.Add(Task);
		TriggerEventWhenTasksComplete(InEvent, Prerequistes, CurrentThreadIfKnown);
	}

	/**
	*	Deletegates for shutdown
	*	@param	Callback - function to call prior to shutting down the taskgraph
	**/
	virtual void AddShutdownCallback(TFunction<void()>& Callback) = 0;

	/**
	*	A (slow) function to call a function on every known thread, both named and workers
	*	@param	Callback - function to call prior to shutting down the taskgraph
	**/
	static void BroadcastSlow_OnlyUseForSpecialPurposes(bool bDoTaskThreads, bool bDoBackgroundThreads, TFunction<void(ENamedThreads::Type CurrentThread)>& Callback);
};

/**
 *	Base class for all tasks.
 *	Tasks go through a very specific life stage progression, and this is verified.
 **/


class FBaseGraphTask
{
public:

	// Allocator for small tasks.
	enum
	{
		/** Total size in bytes for a small task that will use the custom allocator **/
		SMALL_TASK_SIZE = 256
	};
	typedef TLockFreeFixedSizeAllocator_TLSCache<SMALL_TASK_SIZE, PLATFORM_CACHE_LINE_SIZE> TSmallTaskAllocator;
protected:
	/**
	 *	Constructor
	 *	@param InNumberOfPrerequistitesOutstanding; the number of prerequisites outstanding. We actually add one to this to prevent the task from firing while we are setting up the task
	 **/
	FBaseGraphTask(int32 InNumberOfPrerequistitesOutstanding)
		: ThreadToExecuteOn(ENamedThreads::AnyThread)
		, NumberOfPrerequistitesOutstanding(InNumberOfPrerequistitesOutstanding + 1) // + 1 is not a prerequisite, it is a lock to prevent it from executing while it is getting prerequisites, one it is safe to execute, call PrerequisitesComplete
	{
		checkThreadGraph(LifeStage.Increment() == int32(LS_Contructed));
	}
	/**
	 *	Sets the desired execution thread. This is not part of the constructor because this information may not be known quite yet duiring construction.
	 *	@param InThreadToExecuteOn; the desired thread to execute on.
	 **/
	void SetThreadToExecuteOn(ENamedThreads::Type InThreadToExecuteOn)
	{
		ThreadToExecuteOn = InThreadToExecuteOn;
		checkThreadGraph(LifeStage.Increment() == int32(LS_ThreadSet));
	}

	/**
	 *	Indicates that the prerequisites are set up and that the task can be executed as soon as the prerequisites are finished.
	 *	@param NumAlreadyFinishedPrequistes; the number of prerequisites that have not been set up because those tasks had already completed.
	 *	@param bUnlock; if true, let the task execute if it can
	 **/
	void PrerequisitesComplete(ENamedThreads::Type CurrentThread, int32 NumAlreadyFinishedPrequistes, bool bUnlock = true)
	{
		checkThreadGraph(LifeStage.Increment() == int32(LS_PrequisitesSetup));
		int32 NumToSub = NumAlreadyFinishedPrequistes + (bUnlock ? 1 : 0); // the +1 is for the "lock" we set up in the constructor
		if (NumberOfPrerequistitesOutstanding.Subtract(NumToSub) == NumToSub)
		{
			QueueTask(CurrentThread);
		}
	}
	/** destructor, just checks the life stage **/
	virtual ~FBaseGraphTask()
	{
		checkThreadGraph(LifeStage.Increment() == int32(LS_Deconstucted));
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/** Logs a task name that may contain invalid subsequents. Debug only. */
	static void X0_API LogPossiblyInvalidSubsequentsTask(const TCHAR* TaskName);
#endif

	/** Singleton to retrieve the small task allocator **/
	static X0_API TSmallTaskAllocator& GetSmallTaskAllocator();

	/**
	 *	An indication that a prerequisite has been completed. Reduces the number of prerequisites by one and if no prerequisites are outstanding, it queues the task for execution.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 **/
	void ConditionalQueueTask(ENamedThreads::Type CurrentThread)
	{
		if (NumberOfPrerequistitesOutstanding.Decrement() == 0)
		{
			QueueTask(CurrentThread);
		}
	}

private:
	friend class FNamedTaskThread;
	friend class FTaskThreadBase;
	friend class FTaskThreadAnyThread;
	friend class FGraphEvent;
	friend class FTaskGraphImplementation;

	// Subclass API

	/**
	 *	Virtual call to actually execute the task. This should also call the destructor and free any memory.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks.
	 **/
	virtual void ExecuteTask(TArray<FBaseGraphTask*>& NewTasks, ENamedThreads::Type CurrentThread) = 0;

	// API called from other parts of the system

	/**
	 *	Called by the system to execute this task after it has been removed from an internal queue.
	 *	Just checks the life stage and passes off to the virtual ExecuteTask method.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks.
	 **/
	FORCEINLINE void Execute(TArray<FBaseGraphTask*>& NewTasks, ENamedThreads::Type CurrentThread)
	{
		checkThreadGraph(LifeStage.Increment() == int32(LS_Executing));
		ExecuteTask(NewTasks, CurrentThread);
	}

	// Internal Use

	/**
	 *	Queues the task for execution.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 **/
	void QueueTask(ENamedThreads::Type CurrentThreadIfKnown)
	{
		checkThreadGraph(LifeStage.Increment() == int32(LS_Queued));
		FTaskGraphInterface::Get().QueueTask(this, ThreadToExecuteOn, CurrentThreadIfKnown);
	}

	/**	Thread to execute on, can be ENamedThreads::AnyThread to execute on any unnamed thread **/
	ENamedThreads::Type			ThreadToExecuteOn;
	/**	Number of prerequisites outstanding. When this drops to zero, the thread is queued for execution.  **/
	FThreadSafeCounter			NumberOfPrerequistitesOutstanding;


#if !UE_BUILD_SHIPPING
	// Life stage verification
	// Tasks go through 8 steps, in order. In non-final builds, we track them with a thread safe counter and verify that the progression is correct.
	enum ELifeStage
	{
		LS_BaseContructed = 0,
		LS_Contructed,
		LS_ThreadSet,
		LS_PrequisitesSetup,
		LS_Queued,
		LS_Executing,
		LS_Deconstucted,
	};
	FThreadSafeCounter			LifeStage;

#endif
};

/**
 *	A FGraphEvent is a list of tasks waiting for something.
 *	These tasks are call the subsequents.
 *	A graph event is a prerequisite for each of its subsequents.
 *	Graph events have a lifetime managed by reference counting.
 **/
class FGraphEvent
{
public:
	/**
	 *	A factory method to create a graph event.
	 *	@return a reference counted pointer to the new graph event. Note this should be stored in a FGraphEventRef or it will be immediately destroyed!
	**/
	static X0_API FGraphEventRef CreateGraphEvent();

	// the returned event will have ref count zero; be sure to add one!
	static X0_API FGraphEvent* CreateGraphEventWithInlineStorage();
	/**
	 *	Attempts to a new subsequent task. If this event has already fired, false is returned and action must be taken to ensure that the task will still fire even though this event cannot be a prerequisite (because it is already finished).
	 *	@return true if the task was successfully set up as a subsequent. false if the event has already fired.
	**/
	bool AddSubsequent(class FBaseGraphTask* Task)
	{
		return SubsequentList.PushIfNotClosed(Task);
	}

	/**
	 *	Verification function to ensure that nobody was tried to add WaitUntil's outside of the context of execution
	**/
	void CheckDontCompleteUntilIsEmpty()
	{
		checkThreadGraph(!EventsToWaitFor.Num());
	}

	/**
	 *	Delay the firing of this event until the given event fires.
	 *	CAUTION: This is only legal while executing the task associated with this event.
	 *	@param EventToWaitFor event to wait for until we fire.
	**/
	void DontCompleteUntil(FGraphEventRef EventToWaitFor)
	{
		checkThreadGraph(!IsComplete()); // it is not legal to add a DontCompleteUntil after the event has been completed. Basically, this is only legal within a task function.
		new (EventsToWaitFor) FGraphEventRef(EventToWaitFor);
	}

	/**
	*	Sets the thread that you want to execute the null gather task on. This is useful if the thing waiting for this chain to complete is a single, named thread.
	*	CAUTION: This is only legal while executing the task associated with this event.
	*	@param ThreadToDoGatherOn thread and priority to execute null gather task on
	**/
	void SetGatherThreadForDontCompleteUntil(ENamedThreads::Type InThreadToDoGatherOn)
	{
		checkThreadGraph(!IsComplete()); // it is not legal to add a DontCompleteUntil after the event has been completed. Basically, this is only legal within a task function.
		ThreadToDoGatherOn = InThreadToDoGatherOn;
	}

	/**
	 *	"Complete" the event. This grabs the list of subsequents and atomically closes it. Then for each subsequent it reduces the number of prerequisites outstanding and if that drops to zero, the task is queued.
	 *	@param CurrentThreadIfKnown if the current thread is known, provide it here. Otherwise it will be determined via TLS if any task ends up being queued.
	**/
	X0_API void DispatchSubsequents(TArray<FBaseGraphTask*>& NewTasks, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread);

	/**
	 *	Determine if the event has been completed. This can be used to poll for completion.
	 *	@return true if this event has completed.
	 *	CAUTION: If this returns false, the event could still end up completing before this function even returns. In other words, a false return does not mean that event is not yet completed!
	**/
	bool IsComplete() const
	{
		return SubsequentList.IsClosed();
	}


private:
	friend class TRefCountPtr<FGraphEvent>;
	friend class TLockFreeClassAllocator_TLSCache<FGraphEvent, PLATFORM_CACHE_LINE_SIZE>;

	/**
	 *	Internal function to call the destructor and recycle a graph event
	 *	@param ToRecycle; graph event to recycle
	**/
	static X0_API void Recycle(FGraphEvent* ToRecycle);

	/**
	 *	Hidden Constructor
	**/
	friend struct FGraphEventAndSmallTaskStorage;
	FGraphEvent(bool bInInline = false)
		: ThreadToDoGatherOn(ENamedThreads::AnyHiPriThreadHiPriTask)
	{
	}

	/**
	 *	Destructor. Verifies we aren't destroying it prematurely.
	**/
	~FGraphEvent();

	// Interface for TRefCountPtr

public:
	/**
	 *	Increases the reference count
	 *	@return the new reference count
	**/
	uint32 AddRef()
	{
		int32 RefCount = ReferenceCount.Increment();
		checkThreadGraph(RefCount > 0);
		return RefCount;
	}
	/**
	 *	Decreases the reference count and destroys the graph event if it is zero.
	 *	@return the new reference count
	**/
	uint32 Release()
	{
		int32 RefCount = ReferenceCount.Decrement();
		checkThreadGraph(RefCount >= 0);
		if (RefCount == 0)
		{
			Recycle(this);
		}
		return RefCount;
	}


private:

	/** Threadsafe list of subsequents for the event **/
	TClosableLockFreePointerListUnorderedSingleConsumer<FBaseGraphTask, 0>	SubsequentList;
	/** List of events to wait for until firing. This is not thread safe as it is only legal to fill it in within the context of an executing task. **/
	FGraphEventArray														EventsToWaitFor;
	/** Number of outstanding references to this graph event **/
	FThreadSafeCounter														ReferenceCount;
	ENamedThreads::Type														ThreadToDoGatherOn;
};

/**
 The user defined task type can take arguments to a constructor. These arguments (unfortunately) must not be references.
 The API required of TTask:

class FGenericTask
{
	TSomeType	SomeArgument;
public:
	FGenericTask(TSomeType InSomeArgument) // CAUTION!: Must not use references in the constructor args; use pointers instead if you need by reference
		: SomeArgument(InSomeArgument)
	{
		// Usually the constructor doesn't do anything except save the arguments for use in DoWork or GetDesiredThread.
	}
	~FGenericTask()
	{
		// you will be destroyed immediately after you execute. Might as well do cleanup in DoWork, but you could also use a destructor.
	}
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGenericTask, STATGROUP_TaskGraphTasks);
	}

	[static] ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::[named thread or AnyThread];
	}
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		// The arguments are useful for setting up other tasks.
		// Do work here, probably using SomeArgument.
		MyCompletionGraphEvent->DontCompleteUntil(TGraphTask<FSomeChildTask>::CreateTask(NULL,CurrentThread).ConstructAndDispatchWhenReady());
	}
};
**/


/**
 *	TGraphTask
 *	Embeds a user defined task, as exemplified above, for doing the work and provides the functionality for setting up and handling prerequisites and subsequents
 **/
template<typename TTask>
class TGraphTask : public FBaseGraphTask
{
public:
	/**
	 *	This is a helper class returned from the factory. It constructs the embeded task with a set of arguments and sets the task up and makes it ready to execute.
	 *	The task may complete before these routines even return.
	 **/
	class FConstructor
	{
	public:
		/** Passthrough internal task constructor and dispatch. Note! Generally speaking references will not pass through; use pointers */
		template<typename...T>
		FGraphEventRef ConstructAndDispatchWhenReady(T&&... Args)
		{
			new ((void *)&Owner->TaskStorage) TTask(Forward<T>(Args)...);
			return Owner->Setup(Prerequisites, CurrentThreadIfKnown);
		}

		/** Passthrough internal task constructor and hold. */
		template<typename...T>
		TGraphTask* ConstructAndHold(T&&... Args)
		{
			new ((void *)&Owner->TaskStorage) TTask(Forward<T>(Args)...);
			return Owner->Hold(Prerequisites, CurrentThreadIfKnown);
		}

	private:
		friend class TGraphTask;

		/** The task that created me to assist with embeded task construction and preparation. **/
		TGraphTask*						Owner;
		/** The list of prerequisites. **/
		const FGraphEventArray*			Prerequisites;
		/** If known, the current thread.  ENamedThreads::AnyThread is also fine, and if that is the value, we will determine the current thread, as needed, via TLS. **/
		ENamedThreads::Type				CurrentThreadIfKnown;

		/** Constructor, simply saves off the arguments for later use after we actually construct the embeded task. **/
		FConstructor(TGraphTask* InOwner, const FGraphEventArray* InPrerequisites, ENamedThreads::Type InCurrentThreadIfKnown)
			: Owner(InOwner)
			, Prerequisites(InPrerequisites)
			, CurrentThreadIfKnown(InCurrentThreadIfKnown)
		{
		}
		/** Prohibited copy construction **/
		FConstructor(const FConstructor& Other)
		{
			check(0);
		}
		/** Prohibited copy **/
		void operator=(const FConstructor& Other)
		{
			check(0);
		}
	};

	/**
	 *	Factory to create a task and return the helper object to construct the embedded task and set it up for execution.
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@return a temporary helper class which can be used to complete the process.
	**/
	static FConstructor CreateTask(const FGraphEventArray* Prerequisites = NULL, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		int32 NumPrereq = Prerequisites ? Prerequisites->Num() : 0;
		if (sizeof(TGraphTask) <= FBaseGraphTask::SMALL_TASK_SIZE)
		{
			void *Mem = FBaseGraphTask::GetSmallTaskAllocator().Allocate();
			return FConstructor(new (Mem) TGraphTask(TTask::GetSubsequentsMode() == ESubsequentsMode::FireAndForget ? NULL : FGraphEvent::CreateGraphEvent(), NumPrereq), Prerequisites, CurrentThreadIfKnown);
		}
		return FConstructor(new TGraphTask(TTask::GetSubsequentsMode() == ESubsequentsMode::FireAndForget ? NULL : FGraphEvent::CreateGraphEvent(), NumPrereq), Prerequisites, CurrentThreadIfKnown);
	}

	void Unlock(ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		ConditionalQueueTask(CurrentThreadIfKnown);
	}

	FGraphEventRef GetCompletionEvent()
	{
		return Subsequents;
	}

private:
	friend class FConstructor;
	friend class FGraphEvent;

	// API derived from FBaseGraphTask

	/**
	 *	Virtual call to actually execute the task.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks.
	 *	Executes the embedded task.
	 *  Destroys the embedded task.
	 *	Dispatches the subsequents.
	 *	Destroys myself.
	 **/
	virtual void ExecuteTask(TArray<FBaseGraphTask*>& NewTasks, ENamedThreads::Type CurrentThread) final override
	{
		checkThreadGraph(TaskConstructed);

		// Fire and forget mode must not have subsequents
		// Track subsequents mode must have subsequents
		checkThreadGraph(XOR(TTask::GetSubsequentsMode() == ESubsequentsMode::FireAndForget, IsValidRef(Subsequents)));

		if (TTask::GetSubsequentsMode() == ESubsequentsMode::TrackSubsequents)
		{
			Subsequents->CheckDontCompleteUntilIsEmpty(); // we can only add wait for tasks while executing the task
		}

		TTask& Task = *(TTask*)&TaskStorage;
		{
			//FScopeCycleCounter Scope(Task.GetStatId(), true);
			Task.DoTask(CurrentThread, Subsequents);
			Task.~TTask();
			checkThreadGraph(ENamedThreads::GetThreadIndex(CurrentThread) <= ENamedThreads::GetRenderThread() || FMemStack::Get().IsEmpty()); // you must mark and pop memstacks if you use them in tasks! Named threads are excepted.
		}

		TaskConstructed = false;

		if (TTask::GetSubsequentsMode() == ESubsequentsMode::TrackSubsequents)
		{
			FPlatformMisc::MemoryBarrier();
			Subsequents->DispatchSubsequents(NewTasks, CurrentThread);
		}

		if (sizeof(TGraphTask) <= FBaseGraphTask::SMALL_TASK_SIZE)
		{
			this->TGraphTask::~TGraphTask();
			FBaseGraphTask::GetSmallTaskAllocator().Free(this);
		}
		else
		{
			delete this;
		}
	}

	// Internals 

	/**
	 *	Private constructor, constructs the base class with the number of prerequisites.
	 *	@param InSubsequents subsequents to associate with this task. Thsi refernence is destroyed in the process!
	 *	@param NumberOfPrerequistitesOutstanding the number of prerequisites this task will have when it is built.
	**/
	TGraphTask(FGraphEventRef InSubsequents, int32 NumberOfPrerequistitesOutstanding)
		: FBaseGraphTask(NumberOfPrerequistitesOutstanding)
		, TaskConstructed(false)
	{
		Subsequents.Swap(InSubsequents);
	}

	/**
	 *	Private destructor, just checks that the task appears to be completed
	**/
	virtual ~TGraphTask() final override
	{
		checkThreadGraph(!TaskConstructed);
	}

	/**
	 *	Call from FConstructor to complete the setup process
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@param bUnlock; if true, task can execute right now if possible
	 *
	 *	Create the completed event
	 *	Set the thread to execute on based on the embedded task
	 *	Attempt to add myself as a subsequent to each prerequisite
	 *	Tell the base task that I am ready to start as soon as my prerequisites are ready.
	 **/
	void SetupPrereqs(const FGraphEventArray* Prerequisites, ENamedThreads::Type CurrentThreadIfKnown, bool bUnlock)
	{
		checkThreadGraph(!TaskConstructed);
		TaskConstructed = true;
		TTask& Task = *(TTask*)&TaskStorage;
		SetThreadToExecuteOn(Task.GetDesiredThread());
		int32 AlreadyCompletedPrerequisites = 0;
		if (Prerequisites)
		{
			for (int32 Index = 0; Index < Prerequisites->Num(); Index++)
			{
				check((*Prerequisites)[Index]);
				if (!(*Prerequisites)[Index]->AddSubsequent(this))
				{
					AlreadyCompletedPrerequisites++;
				}
			}
		}
		PrerequisitesComplete(CurrentThreadIfKnown, AlreadyCompletedPrerequisites, bUnlock);
	}

	/**
	 *	Call from FConstructor to complete the setup process
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@return A new graph event which represents the completion of this task.
	 *
	 *	Create the completed event
	 *	Set the thread to execute on based on the embedded task
	 *	Attempt to add myself as a subsequent to each prerequisite
	 *	Tell the base task that I am ready to start as soon as my prerequisites are ready.
	 **/
	FGraphEventRef Setup(const FGraphEventArray* Prerequisites = NULL, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		FGraphEventRef ReturnedEventRef = Subsequents; // very important so that this doesn't get destroyed before we return
		SetupPrereqs(Prerequisites, CurrentThreadIfKnown, true);
		return ReturnedEventRef;
	}

	/**
	 *	Call from FConstructor to complete the setup process, but doesn't allow the task to dispatch yet
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@return a pointer to the task
	 *
	 *	Create the completed event
	 *	Set the thread to execute on based on the embedded task
	 *	Attempt to add myself as a subsequent to each prerequisite
	 *	Tell the base task that I am ready to start as soon as my prerequisites are ready.
	 **/
	TGraphTask* Hold(const FGraphEventArray* Prerequisites = NULL, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		SetupPrereqs(Prerequisites, CurrentThreadIfKnown, false);
		return this;
	}

	/**
	 *	Factory to create a gather task which assumes the given subsequent list from some other tasks.
	 *	This is used to support "WaitFor" during a task execution.
	 *	@param SubsequentsToAssume; subsequents to "assume" from an existing task
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to dispatching my seubsequents.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	**/
	static FConstructor CreateTask(FGraphEventRef SubsequentsToAssume, const FGraphEventArray* Prerequisites = NULL, ENamedThreads::Type CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		if (sizeof(TGraphTask) <= FBaseGraphTask::SMALL_TASK_SIZE)
		{
			void *Mem = FBaseGraphTask::GetSmallTaskAllocator().Allocate();
			return FConstructor(new (Mem) TGraphTask(SubsequentsToAssume, Prerequisites ? Prerequisites->Num() : 0), Prerequisites, CurrentThreadIfKnown);
		}
		return FConstructor(new TGraphTask(SubsequentsToAssume, Prerequisites ? Prerequisites->Num() : 0), Prerequisites, CurrentThreadIfKnown);
	}

	/** An aligned bit of storage to hold the embedded task **/
	TAlignedBytes<sizeof(TTask), alignof(TTask)> TaskStorage;
	/** Used to sanity check the state of the object **/
	bool						TaskConstructed;
	/** A reference counted pointer to the completion event which lists the tasks that have me as a prerequisite. **/
	FGraphEventRef				Subsequents;
};

/**
 *	FReturnGraphTask is a task used to return flow control from a named thread back to the original caller of ProcessThreadUntilRequestReturn
 **/
class FReturnGraphTask
{
public:

	/**
	 *	Constructor
	 *	@param InThreadToReturnFrom; named thread to cause to return
	**/
	FReturnGraphTask(ENamedThreads::Type InThreadToReturnFrom)
		: ThreadToReturnFrom(InThreadToReturnFrom)
	{
		checkThreadGraph(ThreadToReturnFrom != ENamedThreads::AnyThread); // doesn't make any sense to return from any thread
	}

// 	FORCEINLINE TStatId GetStatId() const
// 	{
// 		return GET_STATID(STAT_FReturnGraphTask);
// 	}

	/**
	 *	Retrieve the thread that this task wants to run on.
	 *	@return the thread that this task should run on.
	 **/
	ENamedThreads::Type GetDesiredThread()
	{
		return ThreadToReturnFrom;
	}

	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; }

	/**
	 *	Actually execute the task.
	 *	@param	CurrentThread; the thread we are running on
	 *	@param	MyCompletionGraphEvent; my completion event. Not always useful since at the end of DoWork, you can assume you are done and hence further tasks do not need you as a prerequisite.
	 *	However, MyCompletionGraphEvent can be useful for passing to other routines or when it is handy to set up subsequents before you actually do work.
	 **/
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		checkThreadGraph(ENamedThreads::GetThreadIndex(ThreadToReturnFrom) == ENamedThreads::GetThreadIndex(CurrentThread)); // we somehow are executing on the wrong thread.
		FTaskGraphInterface::Get().RequestReturn(ThreadToReturnFrom);
	}

private:
	/** Named thread that we want to cause to return to the caller of ProcessThreadUntilRequestReturn. **/
	ENamedThreads::Type ThreadToReturnFrom;
};

/**
 *	FNullGraphTask is a task that does nothing. It can be used to "gather" tasks into one prerequisite.
 **/
class FNullGraphTask /*: public FCustomStatIDGraphTaskBase*/
{
public:
	/**
	 *	Constructor
	 *	@param StatId The stat id for this task.
	 *	@param InDesiredThread; Thread to run on, can be ENamedThreads::AnyThread
	**/
	FNullGraphTask(/*const TStatId& StatId, */ENamedThreads::Type InDesiredThread)
		: /*FCustomStatIDGraphTaskBase(StatId)
		, */DesiredThread(InDesiredThread)
	{
	}

	/**
	 *	Retrieve the thread that this task wants to run on.
	 *	@return the thread that this task should run on.
	 **/
	ENamedThreads::Type GetDesiredThread()
	{
		return DesiredThread;
	}

	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; }

	/**
	 *	Actually execute the task.
	 *	@param	CurrentThread; the thread we are running on
	 *	@param	MyCompletionGraphEvent; my completion event. Not always useful since at the end of DoWork, you can assume you are done and hence further tasks do not need you as a prerequisite.
	 *	However, MyCompletionGraphEvent can be useful for passing to other routines or when it is handy to set up subsequents before you actually do work.
	 **/
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
	}
private:
	/** Thread to run on, can be ENamedThreads::AnyThread **/
	ENamedThreads::Type DesiredThread;
};

/**
 * FTriggerEventGraphTask is a task that triggers an event
 */
class FTriggerEventGraphTask
{
public:
	/**
	 *	Constructor
	 *	@param InScopedEvent; Scoped event to fire
	**/
	FTriggerEventGraphTask(FEvent* InEvent)
		: Event(InEvent)
	{
		check(Event);
	}

// 	FORCEINLINE TStatId GetStatId() const
// 	{
// 		return GET_STATID(STAT_FTriggerEventGraphTask);
// 	}

	ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::AnyHiPriThreadHiPriTask;
	}

	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; }

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		Event->Trigger();
	}
private:
	FEvent* Event;
};

