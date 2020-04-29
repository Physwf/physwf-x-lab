#pragma once

#include "CoreMinimal.h"
//#include "Stats/Stats.h"

#include "HAL/PlatformTime.h"

#include "X4.h"
/**
 *	Timer helper class.
 **/
class FTimer
{
public:
	/**
	 *	Constructor
	 **/
	FTimer()
		: CurrentDeltaTime(0.0f)
		, CurrentTime(0.0f)
	{
	}

	/**
	 *	Returns the current time, in seconds.
	 *	@return Current time, in seconds
	 */
	float GetCurrentTime() const
	{
		return CurrentTime;
	}

	/**
	 *	Returns the current delta time.
	 *	@return Current delta time (number of seconds that passed between the last two tick)
	 */
	float GetCurrentDeltaTime() const
	{
		return CurrentDeltaTime;
	}

	/**
	 *	Updates the timer.
	 *	@param DeltaTime	Number of seconds that have passed since the last tick
	 **/
	void Tick(float DeltaTime)
	{
		CurrentDeltaTime = DeltaTime;
		CurrentTime += DeltaTime;
	}

protected:
	/** Current delta time (number of seconds that passed between the last two tick). */
	float CurrentDeltaTime;
	/** Current time, in seconds. */
	float CurrentTime;
};


/** Whether to pause the global realtime clock for the rendering thread (read and write only on main thread). */
extern X4_API bool GPauseRenderingRealtimeClock;

/** Global realtime clock for the rendering thread. */
extern X4_API FTimer GRenderingRealtimeClock;

/**
 * Encapsulates a latency timer that measures the time from when mouse input
 * is read on the gamethread until that frame is fully displayed by the GPU.
 */
struct FInputLatencyTimer
{
	/**
	 * Constructor
	 * @param InUpdateFrequency	How often the timer should be updated (in seconds).
	 */
	FInputLatencyTimer(float InUpdateFrequency)
	{
		bInitialized = false;
		RenderThreadTrigger = false;
		GameThreadTrigger = false;
		StartTime = 0;
		DeltaTime = 0;
		LastCaptureTime = 0.0f;
		UpdateFrequency = InUpdateFrequency;
	}

	/** Potentially starts the timer on the gamethread, based on the UpdateFrequency. */
	X4_API void GameThreadTick();

	/** @return The number of seconds of input latency. */
	inline float GetDeltaSeconds() const
	{
		return FPlatformTime::ToSeconds(DeltaTime);
	}

	/** Weather GInputLatencyTimer is initialized or not. */
	bool	bInitialized;

	/** Weather a measurement has been triggered on the gamethread. */
	bool	GameThreadTrigger;

	/** Weather a measurement has been triggered on the renderthread. */
	bool	RenderThreadTrigger;

	/** Start time (in FPlatformTime::Cycles). */
	uint32	StartTime;

	/** Last delta time that was measured (in FPlatformTime::Cycles). */
	uint32	DeltaTime;

	/** Last time we did a measurement (in seconds). */
	float	LastCaptureTime;

	/** How often we should do a measurement (in seconds). */
	float	UpdateFrequency;
};

namespace ERenderThreadIdleTypes
{
	enum Type
	{
		WaitingForAllOtherSleep,
		WaitingForGPUQuery,
		WaitingForGPUPresent,
		Num
	};
}

/** Accumulates how many cycles the renderthread has been idle. It's defined in RenderingThread.cpp. */
extern X4_API uint32 GRenderThreadIdle[ERenderThreadIdleTypes::Num];
/** Accumulates how times renderthread was idle. It's defined in RenderingThread.cpp. */
extern X4_API uint32 GRenderThreadNumIdle[ERenderThreadIdleTypes::Num];
/** Global input latency timer. Defined in UnrealClient.cpp */
extern X4_API FInputLatencyTimer GInputLatencyTimer;
/** How many cycles the renderthread used (excluding idle time). It's set once per frame in FViewport::Draw. */
extern X4_API uint32 GRenderThreadTime;
/** How many cycles the rhithread used (excluding idle time). */
extern X4_API uint32 GRHIThreadTime;
/** How many cycles the gamethread used (excluding idle time). It's set once per frame in FViewport::Draw. */
extern X4_API uint32 GGameThreadTime;
/** How many cycles it took to swap buffers to present the frame. */
extern X4_API uint32 GSwapBufferTime;

// shared by renderer and engine, compiles down to a constant in final release
X4_API int32 GetCVarForceLOD();

X4_API int32 GetCVarForceLODShadow();

X4_API bool IsHDREnabled();