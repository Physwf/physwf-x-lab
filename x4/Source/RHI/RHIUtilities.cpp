#include "CoreMinimal.h"
//#include "HAL/PlatformStackWalk.h"
//#include "HAL/IConsoleManager.h"
#include "RHI/RHI.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "HAL/PlatformTime.h"
#include "Misc/ScopeLock.h"

#define USE_FRAME_OFFSET_THREAD 1


X4_API FRHILockTracker GRHILockTracker;

FString FDumpTransitionsHelper::DumpTransitionForResource = TEXT("");
void FDumpTransitionsHelper::DumpTransitionForResourceHandler()
{
	const FString NewValue = /*CVarDumpTransitionsForResource.GetValueOnGameThread()*/ TEXT("");
	DumpTransitionForResource = FString(*NewValue);
}

void FDumpTransitionsHelper::DumpResourceTransition(const FString& ResourceName, const EResourceTransitionAccess TransitionType)
{
	const FString ResourceDumpName = FDumpTransitionsHelper::DumpTransitionForResource;
// 	if ((ResourceDumpName != NAME_None) && (ResourceDumpName == ResourceName))
// 	{
// 		const uint32 DumpCallstackSize = 2047;
// 		ANSICHAR DumpCallstack[DumpCallstackSize] = { 0 };
// 
// 		FPlatformStackWalk::StackWalkAndDump(DumpCallstack, DumpCallstackSize, 2);
// 		UE_LOG(LogRHI, Log, TEXT("%s transition to: %s"), *ResourceDumpName.ToString(), *FResourceTransitionUtility::ResourceTransitionAccessStrings[(int32)TransitionType]);
// 		UE_LOG(LogRHI, Log, TEXT("%s"), ANSI_TO_TCHAR(DumpCallstack));
// 	}
}

void SetDepthBoundsTest(FRHICommandList& RHICmdList, float WorldSpaceDepthNear, float WorldSpaceDepthFar, const FMatrix& ProjectionMatrix)
{
	if (GSupportsDepthBoundsTest)
	{
		FVector4 Near = ProjectionMatrix.TransformFVector4(FVector4(0, 0, WorldSpaceDepthNear));
		FVector4 Far = ProjectionMatrix.TransformFVector4(FVector4(0, 0, WorldSpaceDepthFar));
		float DepthNear = Near.Z / Near.W;
		float DepthFar = Far.Z / Far.W;

		DepthFar = FMath::Clamp(DepthFar, 0.0f, 1.0f);
		DepthNear = FMath::Clamp(DepthNear, 0.0f, 1.0f);

		if (DepthNear <= DepthFar)
		{
			DepthNear = 1.0f;
			DepthFar = 0.0f;
		}

		// Note, using a reversed z depth surface
		RHICmdList.SetDepthBounds(DepthFar, DepthNear);
	}
}

class FRHIFrameFlipTrackingRunnable : public FRunnable
{
	static FRunnableThread* Thread;
	static FRHIFrameFlipTrackingRunnable Singleton;
	static bool bInitialized;

	FCriticalSection CS;
	struct FFramePair
	{
		uint64 PresentIndex;
		FGraphEventRef Event;
	};
	TArray<FFramePair> FramePairs;

	bool bRun;

	FRHIFrameFlipTrackingRunnable();

	virtual uint32 Run() override;
	virtual void Stop() override;

public:
	static void Initialize();
	static void Shutdown();

	static void CompleteGraphEventOnFlip(uint64 PresentIndex, FGraphEventRef Event);
};

FRHIFrameFlipTrackingRunnable::FRHIFrameFlipTrackingRunnable()
	: bRun(true)
{}

#if USE_FRAME_OFFSET_THREAD
struct FRHIFrameOffsetThread : public FRunnable
{
	static FRunnableThread* Thread;
	static FRHIFrameOffsetThread Singleton;
	static bool bInitialized;

	FCriticalSection CS;
	FRHIFlipDetails LastFlipFrame;

	bool bRun;

	FEvent* WaitEvent;

	virtual uint32 Run() override
	{
		while (bRun)
		{
			FRHIFlipDetails NewFlipFrame = GDynamicRHI->RHIWaitForFlip(-1);

			int32 SyncInterval = RHIGetSyncInterval();
			double TargetFrameTimeInSeconds = double(SyncInterval) / 60.0;
			double SlackInSeconds = /*CVarRHISyncSlackMS.GetValueOnAnyThread()*/10 / 1000.0;
			double TargetFlipTime = (NewFlipFrame.VBlankTimeInSeconds + TargetFrameTimeInSeconds) - SlackInSeconds;

			double Timeout = FMath::Max(0.0, TargetFlipTime - FPlatformTime::Seconds());

			FPlatformProcess::Sleep(Timeout);

			{
				FScopeLock Lock(&CS);
				LastFlipFrame = NewFlipFrame;
				LastFlipFrame.FlipTimeInSeconds = LastFlipFrame.FlipTimeInSeconds + TargetFrameTimeInSeconds - SlackInSeconds;
				LastFlipFrame.VBlankTimeInSeconds = LastFlipFrame.VBlankTimeInSeconds + TargetFrameTimeInSeconds - SlackInSeconds;
				LastFlipFrame.PresentIndex++;
			}

			WaitEvent->Trigger();
		}

		return 0;
	}

	virtual void Stop() override
	{
		bRun = false;
	}

public:
	FRHIFrameOffsetThread()
		: bRun(true)
		, WaitEvent(nullptr)
	{}

	~FRHIFrameOffsetThread()
	{
		if (WaitEvent)
		{
			FPlatformProcess::ReturnSynchEventToPool(WaitEvent);
			WaitEvent = nullptr;
		}
	}

	static FRHIFlipDetails WaitForFlip(double Timeout)
	{
		check(Singleton.WaitEvent);
		Singleton.WaitEvent->Wait((uint32)(Timeout * 1000.0));

		FScopeLock Lock(&Singleton.CS);
		return Singleton.LastFlipFrame;
	}

	static void Signal()
	{
		Singleton.WaitEvent->Trigger();
	}

	static void Initialize()
	{
		bInitialized = true;
		Singleton.GetOrInitializeWaitEvent();
		check(Thread == nullptr);
		Thread = FRunnableThread::Create(&Singleton, TEXT("RHIFrameOffsetThread"), 0, TPri_AboveNormal);
	}

	static void Shutdown()
	{
		// PS4 calls shutdown before initialize has been called, so bail out if that happens  
		if (!bInitialized)
		{
			return;
		}
		bInitialized = false;
		GDynamicRHI->RHISignalFlipEvent();

		if (Thread)
		{
			Thread->Kill(true);
			delete Thread;
			Thread = nullptr;
		}

		Singleton.GetOrInitializeWaitEvent()->Trigger();
	}

private:
	FEvent* GetOrInitializeWaitEvent()
	{
		// Wait event can't be initialized with the singleton, because it will crash when initialized to early
		if (WaitEvent == nullptr)
		{
			WaitEvent = FPlatformProcess::GetSynchEventFromPool(false);
		}
		return WaitEvent;
	}

};

FRunnableThread* FRHIFrameOffsetThread::Thread = nullptr;
FRHIFrameOffsetThread FRHIFrameOffsetThread::Singleton;
bool FRHIFrameOffsetThread::bInitialized = false;

#endif // USE_FRAME_OFFSET_THREAD

uint32 FRHIFrameFlipTrackingRunnable::Run()
{
	uint64 SyncFrame = 0;
	double SyncTime = FPlatformTime::Seconds();
	bool bForceFlipSync = true;

	while (bRun)
	{
		// Determine the next expected flip time, based on the previous flip time we synced to.
		int32 SyncInterval = RHIGetSyncInterval();
		double TargetFrameTimeInSeconds = double(SyncInterval) / 60.0;
		double ExpectedNextFlipTimeInSeconds = SyncTime + (TargetFrameTimeInSeconds * 1.02); // Add 2% to prevent early timeout
		double CurrentTimeInSeconds = FPlatformTime::Seconds();

		double TimeoutInSeconds = (SyncInterval == 0 || bForceFlipSync) ? -1.0 : FMath::Max(ExpectedNextFlipTimeInSeconds - CurrentTimeInSeconds, 0.0);

#if USE_FRAME_OFFSET_THREAD
		FRHIFlipDetails FlippedFrame = FRHIFrameOffsetThread::WaitForFlip(TimeoutInSeconds);
#else
		FRHIFlipDetails FlippedFrame = GDynamicRHI->RHIWaitForFlip(TimeoutInSeconds);
#endif

		CurrentTimeInSeconds = FPlatformTime::Seconds();
		if (FlippedFrame.PresentIndex > SyncFrame)
		{
			// A new frame has flipped
			SyncFrame = FlippedFrame.PresentIndex;
			SyncTime = FlippedFrame.VBlankTimeInSeconds;
			bForceFlipSync = /*CVarRHISyncAllowEarlyKick.GetValueOnAnyThread()*/1 == 0;
		}
		else if (SyncInterval != 0 && !bForceFlipSync && (CurrentTimeInSeconds > ExpectedNextFlipTimeInSeconds))
		{
			// We've missed a flip. Signal the next frame
			// anyway to optimistically recover from a hitch.
			SyncFrame = FlippedFrame.PresentIndex + 1;
			SyncTime = CurrentTimeInSeconds;
		}

		// Complete any relevant task graph events.
		FScopeLock Lock(&CS);
		for (int32 PairIndex = FramePairs.Num() - 1; PairIndex >= 0; --PairIndex)
		{
			auto const& Pair = FramePairs[PairIndex];
			if (Pair.PresentIndex <= SyncFrame)
			{
				// "Complete" the task graph event
				TArray<FBaseGraphTask*> Subsequents;
				Pair.Event->DispatchSubsequents(Subsequents);

				FramePairs.RemoveAtSwap(PairIndex);
			}
		}
	}

	return 0;
}

void FRHIFrameFlipTrackingRunnable::Stop()
{
	bRun = false;
#if USE_FRAME_OFFSET_THREAD
	FRHIFrameOffsetThread::Signal();
#else
	GDynamicRHI->RHISignalFlipEvent();
#endif
}

void FRHIFrameFlipTrackingRunnable::Initialize()
{
	check(Thread == nullptr);
	bInitialized = true;
	Thread = FRunnableThread::Create(&Singleton, TEXT("RHIFrameFlipThread"), 0, TPri_AboveNormal);
}

void FRHIFrameFlipTrackingRunnable::Shutdown()
{
	if (!bInitialized)
	{
		return;
	}
	bInitialized = false;
	if (Thread)
	{
		Thread->Kill(true);
		delete Thread;
		Thread = nullptr;
	}

	FScopeLock Lock(&Singleton.CS);

	// Signal any remaining events
	for (auto const& Pair : Singleton.FramePairs)
	{
		// "Complete" the task graph event
		TArray<FBaseGraphTask*> Subsequents;
		Pair.Event->DispatchSubsequents(Subsequents);
	}

	Singleton.FramePairs.Empty();

#if USE_FRAME_OFFSET_THREAD
	FRHIFrameOffsetThread::Shutdown();
#endif
}

void FRHIFrameFlipTrackingRunnable::CompleteGraphEventOnFlip(uint64 PresentIndex, FGraphEventRef Event)
{
	FScopeLock Lock(&Singleton.CS);

	if (Thread)
	{
		FFramePair Pair;
		Pair.PresentIndex = PresentIndex;
		Pair.Event = Event;

		Singleton.FramePairs.Add(Pair);

#if USE_FRAME_OFFSET_THREAD
		FRHIFrameOffsetThread::Signal();
#else
		GDynamicRHI->RHISignalFlipEvent();
#endif
	}
	else
	{
		// Platform does not support flip tracking.
		// Signal the event now...

		TArray<FBaseGraphTask*> Subsequents;
		Event->DispatchSubsequents(Subsequents);
	}
}

FRunnableThread* FRHIFrameFlipTrackingRunnable::Thread;
FRHIFrameFlipTrackingRunnable FRHIFrameFlipTrackingRunnable::Singleton;
bool FRHIFrameFlipTrackingRunnable::bInitialized = false;

X4_API uint32 RHIGetSyncInterval()
{
	return /*CVarRHISyncInterval.GetValueOnAnyThread()*/1;
}

X4_API void RHIGetPresentThresholds(float& OutTopPercent, float& OutBottomPercent)
{
	OutTopPercent = FMath::Clamp(/*CVarRHIPresentThresholdTop.GetValueOnAnyThread()*/0.0f, 0.0f, 1.0f);
	OutBottomPercent = FMath::Clamp(/*CVarRHIPresentThresholdBottom.GetValueOnAnyThread()*/0.0f, 0.0f, 1.0f);
}

X4_API void RHICompleteGraphEventOnFlip(uint64 PresentIndex, FGraphEventRef Event)
{
	FRHIFrameFlipTrackingRunnable::CompleteGraphEventOnFlip(PresentIndex, Event);
}

X4_API void RHIInitializeFlipTracking()
{
#if USE_FRAME_OFFSET_THREAD
	FRHIFrameOffsetThread::Initialize();
#endif
	FRHIFrameFlipTrackingRunnable::Initialize();
}

X4_API void RHIShutdownFlipTracking()
{
	FRHIFrameFlipTrackingRunnable::Shutdown();
#if USE_FRAME_OFFSET_THREAD
	FRHIFrameOffsetThread::Shutdown();
#endif
}