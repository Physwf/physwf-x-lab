#pragma once

#include "CoreTypes.h"
#include "Templates/UnrealTemplate.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
//#include "Misc/Parse.h"
//#include "UObject/NameTypes.h"
#include "CoreGlobals.h"
//#include "Delegates/Delegate.h"
#include "Misc/Guid.h"
#include "Misc/CoreMisc.h"
//#include "Misc/CommandLine.h"
//#include "Misc/Timecode.h"
#include "HAL/PlatformProcess.h"

#include "X0.h"

// platforms which can have runtime threading switches
#define HAVE_RUNTIME_THREADING_SWITCHES			(PLATFORM_DESKTOP || PLATFORM_ANDROID || PLATFORM_IOS || PLATFORM_HTML5)


/**
 * Provides information about the application.
 */
class X0_API FApp
{
public:

	/**
	 * Gets whether the application is running with Debug game libraries (set from commandline)
	 */
	static bool IsRunningDebug();


	/**
	 * Gets the globally unique identifier of this application instance.
	 *
	 * Every running instance of the engine has a globally unique instance identifier
	 * that can be used to identify it from anywhere on the network.
	 *
	 * @return Instance identifier, or an invalid GUID if there is no local instance.
	 * @see GetSessionId
	 */
	FORCEINLINE static FGuid GetInstanceId()
	{
		return InstanceId;
	}

	/**
	 * Gets the identifier of the session that this application is part of.
	 *
	 * A session is group of applications that were launched and logically belong together.
	 * For example, when starting a new session in UFE that launches a game on multiple devices,
	 * all engine instances running on those devices will have the same session identifier.
	 * Conversely, sessions that were launched separately will have different session identifiers.
	 *
	 * @return Session identifier, or an invalid GUID if there is no local instance.
	 * @see GetInstanceId
	 */
	FORCEINLINE static FGuid GetSessionId()
	{
		return SessionId;
	}

	/**
	 * Gets the name of the session that this application is part of, if any.
	 *
	 * @return Session name string.
	 */
	FORCEINLINE static FString GetSessionName()
	{
		return SessionName;
	}

	/**
	 * Gets the name of the user who owns the session that this application is part of, if any.
	 *
	 * If this application is part of a session that was launched from UFE, this function
	 * will return the name of the user that launched the session. If this application is
	 * not part of a session, this function will return the name of the local user account
	 * under which the application is running.
	 *
	 * @return Name of session owner.
	 */
	FORCEINLINE static FString GetSessionOwner()
	{
		return SessionOwner;
	}

	/**
	 * Initializes the application session.
	 */
	static void InitializeSession();

	/**
 * Set a new session name.
 *
 * @param NewName The new session name.
 * @see SetSessionOwner
 */
	FORCEINLINE static void SetSessionName(const FString& NewName)
	{
		SessionName = NewName;
	}

	/**
	 * Set a new session owner.
	 *
	 * @param NewOwner The name of the new owner.
	 * @see SetSessionName
	 */
	FORCEINLINE static void SetSessionOwner(const FString& NewOwner)
	{
		SessionOwner = NewOwner;
	}

public:

	/**
	 * Checks whether this application can render anything.
	 * Certain application types never render, while for others this behavior may be controlled by switching to NullRHI.
	 * This can be used for decisions like omitting code paths that make no sense on servers or games running in headless mode (e.g. automated tests).
	 *
	 * @return true if the application can render, false otherwise.
	 */
	FORCEINLINE static bool CanEverRender()
	{
#if UE_SERVER
		return false;
#else
// 		static bool bHasNullRHIOnCommandline = FParse::Param(FCommandLine::Get(), TEXT("nullrhi"));
// 		return (!IsRunningCommandlet() || IsAllowCommandletRendering()) && !IsRunningDedicatedServer() && !(USE_NULL_RHI || bHasNullRHIOnCommandline);
		return true;
#endif // UE_SERVER
	}

	/**
	 * Gets time step in seconds if a fixed delta time is wanted.
	 *
	 * @return Time step in seconds for fixed delta time.
	 */
	FORCEINLINE static double GetFixedDeltaTime()
	{
		return FixedDeltaTime;
	}

	/**
	 * Sets time step in seconds if a fixed delta time is wanted.
	 *
	 * @param seconds Time step in seconds for fixed delta time.
	 */
	static void SetFixedDeltaTime(double Seconds)
	{
		FixedDeltaTime = Seconds;
	}

	/**
	 * Gets whether we want to use a fixed time step or not.
	 *
	 * @return True if using fixed time step, false otherwise.
	 */
	static bool UseFixedTimeStep()
	{
		return bUseFixedTimeStep;
	}

	/**
	 * Enables or disabled usage of fixed time step.
	 *
	 * @param whether to use fixed time step or not
	 */
	static void SetUseFixedTimeStep(bool bVal)
	{
		bUseFixedTimeStep = bVal;
	}

	/**
	 * Gets current time in seconds.
	 *
	 * @return Current time in seconds.
	 */
	FORCEINLINE static double GetCurrentTime()
	{
		return CurrentTime;
	}

	/**
	 * Sets current time in seconds.
	 *
	 * @param seconds - Time in seconds.
	 */
	static void SetCurrentTime(double Seconds)
	{
		CurrentTime = Seconds;
	}

	/**
	 * Gets previous value of CurrentTime.
	 *
	 * @return Previous value of CurrentTime.
	 */
	FORCEINLINE static double GetLastTime()
	{
		return LastTime;
	}

	/** Updates Last time to CurrentTime. */
	static void UpdateLastTime()
	{
		LastTime = CurrentTime;
	}

	/**
	 * Gets time delta in seconds.
	 *
	 * @return Time delta in seconds.
	 */
	FORCEINLINE static double GetDeltaTime()
	{
		return DeltaTime;
	}

	/**
	 * Sets time delta in seconds.
	 *
	 * @param seconds Time in seconds.
	 */
	static void SetDeltaTime(double Seconds)
	{
		DeltaTime = Seconds;
	}

	/**
	 * Gets idle time in seconds.
	 *
	 * @return Idle time in seconds.
	 */
	FORCEINLINE static double GetIdleTime()
	{
		return IdleTime;
	}

	/**
	 * Sets idle time in seconds.
	 *
	 * @param seconds - Idle time in seconds.
	 */
	static void SetIdleTime(double Seconds)
	{
		IdleTime = Seconds;
	}

	/**
	 * Gets idle time overshoot in seconds (the time beyond the wait time we requested for the frame). Only valid when IdleTime is > 0.
	 *
	 * @return Idle time in seconds.
	 */
	FORCEINLINE static double GetIdleTimeOvershoot()
	{
		return IdleTimeOvershoot;
	}

	/**
	 * Sets idle time overshoot in seconds (the time beyond the wait time we requested for the frame). Only valid when IdleTime is > 0.
	 *
	 * @param seconds - Idle time in seconds.
	 */
	static void SetIdleTimeOvershoot(double Seconds)
	{
		IdleTimeOvershoot = Seconds;
	}

	static bool bUseFixedSeed;

private:
	/** Holds the instance identifier. */
	static FGuid InstanceId;

	/** Holds the session identifier. */
	static FGuid SessionId;

	/** Holds the session name. */
	static FString SessionName;

	/** Holds the name of the user that launched session. */
	static FString SessionOwner;

	/** List of authorized session users. */
	static TArray<FString> SessionUsers;

	/** Holds a flag indicating whether this is a standalone session. */
	static bool Standalone;

	/** Holds a flag Whether we are in benchmark mode or not. */
	static bool bIsBenchmarking;

	/** Holds a flag whether we want to use a fixed time step or not. */
	static bool bUseFixedTimeStep;

	/** Holds time step if a fixed delta time is wanted. */
	static double FixedDeltaTime;

	/** Holds current time. */
	static double CurrentTime;

	/** Holds previous value of CurrentTime. */
	static double LastTime;

	/** Holds current delta time in seconds. */
	static double DeltaTime;

	/** Holds time we spent sleeping in UpdateTimeAndHandleMaxTickRate() if our frame time was smaller than one allowed by target FPS. */
	static double IdleTime;

	/** Holds the amount of IdleTime that was LONGER than we tried to sleep. The OS can't sleep the exact amount of time, so this measures that overshoot. */
	static double IdleTimeOvershoot;

	/** Holds the current timecode. */
	//static FTimecode Timecode;

	/** Use to affect the app volume when it loses focus */
	static float VolumeMultiplier;

	/** Read from config to define the volume when app loses focus */
	static float UnfocusedVolumeMultiplier;

	/** Holds a flag indicating if VRFocus should be used */
	static bool bUseVRFocus;

	/** Holds a flag indicating if app has focus in side the VR headset */
	static bool bHasVRFocus;
};

/**
 * @return true if there is a running game world that is a server (including listen servers), false otherwise
 */
X0_API bool IsServerForOnlineSubsystems(FName WorldContextHandle);

/**
 * Sets the delegate used for IsServerForOnlineSubsystems
 */
//X0_API void SetIsServerForOnlineSubsystemsDelegate(FQueryIsRunningServer NewDelegate);