#include "HAL/ThreadingBase.h"
//#include "UObject/NameTypes.h"
//#include "Stats/Stats.h"
//#include "Misc/CoreStats.h"
#include "Misc/EventPool.h"
#include "Templates/Atomic.h"

#include "X0.h"

X0_API TAtomic<int32> GIsRenderingThreadSuspended(0);

X0_API FRunnableThread* GRenderingThread = nullptr;

X0_API bool IsInActualRenderingThread()
{
	return GRenderingThread && FPlatformTLS::GetCurrentThreadId() == GRenderingThread->GetThreadID();
}

X0_API bool IsInRenderingThread()
{
	return !GRenderingThread || GIsRenderingThreadSuspended.Load(EMemoryOrder::Relaxed) || (FPlatformTLS::GetCurrentThreadId() == GRenderingThread->GetThreadID());
}
/**
* Fake thread created when multi-threading is disabled.
*/
class FFakeThread : public FRunnableThread
{
	/** Thread Id pool */
	static uint32 ThreadIdCounter;

	/** Thread is suspended. */
	bool bIsSuspended;

	/** Runnable object associated with this thread. */
	FSingleThreadRunnable* Runnable;

public:

	/** Constructor. */
	FFakeThread()
		: bIsSuspended(false)
		, Runnable(nullptr)
	{
		ThreadID = ThreadIdCounter++;
		// Auto register with single thread manager.
		FThreadManager::Get().AddThread(ThreadID, this);
	}

	/** Virtual destructor. */
	virtual ~FFakeThread()
	{
		// Remove from the manager.
		FThreadManager::Get().RemoveThread(this);
	}

	/** Tick one time per frame. */
	virtual void Tick() override
	{
		if (Runnable && !bIsSuspended)
		{
			Runnable->Tick();
		}
	}

public:

	// FRunnableThread interface

	virtual void SetThreadPriority(EThreadPriority NewPriority) override
	{
		// Not relevant.
	}

	virtual void Suspend(bool bShouldPause) override
	{
		bIsSuspended = bShouldPause;
	}

	virtual bool Kill(bool bShouldWait) override
	{
		FThreadManager::Get().RemoveThread(this);
		return true;
	}

	virtual void WaitForCompletion() override
	{
		FThreadManager::Get().RemoveThread(this);
	}

	virtual bool CreateInternal(FRunnable* InRunnable, const TCHAR* InThreadName,
		uint32 InStackSize,
		EThreadPriority InThreadPri, uint64 InThreadAffinityMask) override

	{
		Runnable = InRunnable->GetSingleThreadInterface();
		if (Runnable)
		{
			InRunnable->Init();
		}
		return Runnable != nullptr;
	}
};
uint32 FFakeThread::ThreadIdCounter = 0xffff;

void FThreadManager::AddThread(uint32 ThreadId, FRunnableThread* Thread)
{
	FScopeLock ThreadsLock(&ThreadsCritical);
	// Some platforms do not support TLS
	if (!Threads.Contains(ThreadId))
	{
		Threads.Add(ThreadId, Thread);
	}
}

void FThreadManager::RemoveThread(FRunnableThread* Thread)
{
	FScopeLock ThreadsLock(&ThreadsCritical);
	const uint32* ThreadId = Threads.FindKey(Thread);
	if (ThreadId)
	{
		Threads.Remove(*ThreadId);
	}
}

void FThreadManager::Tick()
{
	if (!FPlatformProcess::SupportsMultithreading())
	{
		//QUICK_SCOPE_CYCLE_COUNTER(STAT_FSingleThreadManager_Tick);

		FScopeLock ThreadsLock(&ThreadsCritical);

		// Tick all registered threads.
		for (TPair<uint32, FRunnableThread*>& ThreadPair : Threads)
		{
			ThreadPair.Value->Tick();
		}
	}
}

const FString& FThreadManager::GetThreadName(uint32 ThreadId)
{
	static FString NoThreadName;
	FScopeLock ThreadsLock(&ThreadsCritical);
	FRunnableThread** Thread = Threads.Find(ThreadId);
	if (Thread)
	{
		return (*Thread)->GetThreadName();
	}
	return NoThreadName;
}

FThreadManager& FThreadManager::Get()
{
	static FThreadManager Singleton;
	return Singleton;
}

/*-----------------------------------------------------------------------------
FEvent, FScopedEvent
-----------------------------------------------------------------------------*/

TAtomic<uint32> FEvent::EventUniqueId;

void FEvent::AdvanceStats()
{
#if	STATS
	EventId = EventUniqueId++;
	EventStartCycles = 0;
#endif // STATS
}

void FEvent::WaitForStats()
{
#if	STATS
	// Only start counting on the first wait, trigger will "close" the history.
	if (FThreadStats::IsCollectingData() && EventStartCycles.Load(EMemoryOrder::Relaxed) == 0)
	{
		const uint64 PacketEventIdAndCycles = ((uint64)EventId << 32) | 0;
		STAT_ADD_CUSTOMMESSAGE_PTR(STAT_EventWaitWithId, PacketEventIdAndCycles);
		EventStartCycles = FPlatformTime::Cycles();
	}
#endif // STATS
}

void FEvent::TriggerForStats()
{
#if	STATS
	// Only add wait-trigger pairs.
	uint32 LocalEventStartCycles = EventStartCycles.Load(EMemoryOrder::Relaxed);
	if (LocalEventStartCycles > 0 && FThreadStats::IsCollectingData())
	{
		const uint32 EndCycles = FPlatformTime::Cycles();
		const int32 DeltaCycles = int32(EndCycles - LocalEventStartCycles);
		const uint64 PacketEventIdAndCycles = ((uint64)EventId << 32) | DeltaCycles;
		STAT_ADD_CUSTOMMESSAGE_PTR(STAT_EventTriggerWithId, PacketEventIdAndCycles);

		AdvanceStats();
	}
#endif // STATS
}

void FEvent::ResetForStats()
{
#if	STATS
	AdvanceStats();
#endif // STATS
}

FScopedEvent::FScopedEvent()
	: Event(FEventPool<EEventPoolTypes::AutoReset>::Get().GetEventFromPool())
{ }

bool FScopedEvent::IsReady()
{
	if (Event)
	{
		if (Event->Wait(1))
		{
			FEventPool<EEventPoolTypes::AutoReset>::Get().ReturnToPool(Event);
			Event = nullptr;
			return true;
		}
		return false;
	}
	return true;
}

FScopedEvent::~FScopedEvent()
{
	if (Event)
	{
		Event->Wait();
		FEventPool<EEventPoolTypes::AutoReset>::Get().ReturnToPool(Event);
		Event = nullptr;
	}
}


/*-----------------------------------------------------------------------------
FRunnableThread
-----------------------------------------------------------------------------*/

uint32 FRunnableThread::RunnableTlsSlot = FRunnableThread::GetTlsSlot();

uint32 FRunnableThread::GetTlsSlot()
{
	check(IsInGameThread());
	uint32 TlsSlot = FPlatformTLS::AllocTlsSlot();
	check(FPlatformTLS::IsValidTlsSlot(TlsSlot));
	return TlsSlot;
}

FRunnableThread::FRunnableThread()
	: Runnable(nullptr)
	, ThreadInitSyncEvent(nullptr)
	, ThreadAffinityMask(FPlatformAffinity::GetNoAffinityMask())
	, ThreadPriority(TPri_Normal)
	, ThreadID(0)
{
}

FRunnableThread::~FRunnableThread()
{
	if (!GIsRequestingExit)
	{
		FThreadManager::Get().RemoveThread(this);
	}
}

FRunnableThread* FRunnableThread::Create(
	class FRunnable* InRunnable,
	const TCHAR* ThreadName,
	uint32 InStackSize,
	EThreadPriority InThreadPri,
	uint64 InThreadAffinityMask)
{
	FRunnableThread* NewThread = nullptr;
	if (FPlatformProcess::SupportsMultithreading())
	{
		check(InRunnable);
		// Create a new thread object
		NewThread = FPlatformProcess::CreateRunnableThread();
		if (NewThread)
		{
			// Call the thread's create method
			if (NewThread->CreateInternal(InRunnable, ThreadName, InStackSize, InThreadPri, InThreadAffinityMask) == false)
			{
				// We failed to start the thread correctly so clean up
				delete NewThread;
				NewThread = nullptr;
			}
		}
	}
	else if (InRunnable->GetSingleThreadInterface())
	{
		// Create a fake thread when multithreading is disabled.
		NewThread = new FFakeThread();
		if (NewThread->CreateInternal(InRunnable, ThreadName, InStackSize, InThreadPri) == false)
		{
			// We failed to start the thread correctly so clean up
			delete NewThread;
			NewThread = nullptr;
		}
	}

#if	STATS
	if (NewThread)
	{
		FStartupMessages::Get().AddThreadMetadata(FName(*NewThread->GetThreadName()), NewThread->GetThreadID());
	}
#endif // STATS

	return NewThread;
}

void FRunnableThread::SetTls()
{
	// Make sure it's called from the owning thread.
	check(ThreadID == FPlatformTLS::GetCurrentThreadId());
	check(FPlatformTLS::IsValidTlsSlot(RunnableTlsSlot));
	FPlatformTLS::SetTlsValue(RunnableTlsSlot, this);
}

void FRunnableThread::FreeTls()
{
	// Make sure it's called from the owning thread.
	check(ThreadID == FPlatformTLS::GetCurrentThreadId());
	check(FPlatformTLS::IsValidTlsSlot(RunnableTlsSlot));
	FPlatformTLS::SetTlsValue(RunnableTlsSlot, nullptr);

	// Delete all FTlsAutoCleanup objects created for this thread.
	for (auto& Instance : TlsInstances)
	{
		delete Instance;
		Instance = nullptr;
	}
}


/*-----------------------------------------------------------------------------
FThreadSingletonInitializer
-----------------------------------------------------------------------------*/

FTlsAutoCleanup* FThreadSingletonInitializer::Get(TFunctionRef<FTlsAutoCleanup*()> CreateInstance, uint32& TlsSlot)
{
	if (TlsSlot == 0xFFFFFFFF)
	{
		const uint32 ThisTlsSlot = FPlatformTLS::AllocTlsSlot();
		check(FPlatformTLS::IsValidTlsSlot(ThisTlsSlot));
		const uint32 PrevTlsSlot = FPlatformAtomics::InterlockedCompareExchange((int32*)&TlsSlot, (int32)ThisTlsSlot, 0xFFFFFFFF);
		if (PrevTlsSlot != 0xFFFFFFFF)
		{
			FPlatformTLS::FreeTlsSlot(ThisTlsSlot);
		}
	}
	FTlsAutoCleanup* ThreadSingleton = (FTlsAutoCleanup*)FPlatformTLS::GetTlsValue(TlsSlot);
	if (!ThreadSingleton)
	{
		ThreadSingleton = CreateInstance();
		ThreadSingleton->Register();
		FPlatformTLS::SetTlsValue(TlsSlot, ThreadSingleton);
	}
	return ThreadSingleton;
}

void FTlsAutoCleanup::Register()
{
	FRunnableThread* RunnableThread = FRunnableThread::GetRunnableThread();
	if (RunnableThread)
	{
		RunnableThread->TlsInstances.Add(this);
	}
}
