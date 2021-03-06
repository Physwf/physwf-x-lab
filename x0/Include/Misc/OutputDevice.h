#pragma once

#include "CoreTypes.h"
#include "CoreFwd.h"
#include "Logging/LogVerbosity.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/VarArgs.h"
#include "Templates/IsValidVariadicFunctionArg.h"
#include "Templates/AndOrNot.h"
#include "Templates/IsArrayOrRefOfType.h"

#include "X0.h"

#if !PLATFORM_DESKTOP
// don't support colorized text on consoles
#define SET_WARN_COLOR(Color)
#define SET_WARN_COLOR_AND_BACKGROUND(Color, Bkgrnd)
#define CLEAR_WARN_COLOR() 
#else
/*-----------------------------------------------------------------------------
Colorized text.

To use colored text from a commandlet, you use the SET_WARN_COLOR macro with
one of the following standard colors. Then use CLEAR_WARN_COLOR to return
to default.

To use the standard colors, you simply do this:
SET_WARN_COLOR(COLOR_YELLOW);

You can specify a background color by appending it to the foreground:
SET_WARN_COLOR, COLOR_YELLOW COLOR_DARK_RED);

This will have bright yellow text on a dark red background.

Or you can make your own in the format:

ForegroundRed | ForegroundGreen | ForegroundBlue | ForegroundBright | BackgroundRed | BackgroundGreen | BackgroundBlue | BackgroundBright
where each value is either 0 or 1 (can leave off trailing 0's), so
blue on bright yellow is "00101101" and red on black is "1"

An empty string reverts to the normal gray on black.
-----------------------------------------------------------------------------*/

// putting them in a namespace to protect against future name conflicts
namespace OutputDeviceColor
{
	const TCHAR* const COLOR_BLACK = TEXT("0000");

	const TCHAR* const COLOR_DARK_RED = TEXT("1000");
	const TCHAR* const COLOR_DARK_GREEN = TEXT("0100");
	const TCHAR* const COLOR_DARK_BLUE = TEXT("0010");
	const TCHAR* const COLOR_DARK_YELLOW = TEXT("1100");
	const TCHAR* const COLOR_DARK_CYAN = TEXT("0110");
	const TCHAR* const COLOR_DARK_PURPLE = TEXT("1010");
	const TCHAR* const COLOR_DARK_WHITE = TEXT("1110");
	const TCHAR* const COLOR_GRAY = COLOR_DARK_WHITE;

	const TCHAR* const COLOR_RED = TEXT("1001");
	const TCHAR* const COLOR_GREEN = TEXT("0101");
	const TCHAR* const COLOR_BLUE = TEXT("0011");
	const TCHAR* const COLOR_YELLOW = TEXT("1101");
	const TCHAR* const COLOR_CYAN = TEXT("0111");
	const TCHAR* const COLOR_PURPLE = TEXT("1011");
	const TCHAR* const COLOR_WHITE = TEXT("1111");

	const TCHAR* const COLOR_NONE = TEXT("");
}

using namespace OutputDeviceColor;

// let a console or (UE_BUILD_SHIPPING || UE_BUILD_TEST) define it to nothing
#ifndef SET_WARN_COLOR

/**
* Set the console color with Color or a Color and Background color
*/
#define SET_WARN_COLOR(Color) \
	UE_LOG(LogHAL, SetColor, TEXT("%s"), Color);
#define SET_WARN_COLOR_AND_BACKGROUND(Color, Bkgrnd) \
	UE_LOG(LogHAL, SetColor, TEXT("%s%s"), Color, Bkgrnd);

/**
* Return color to it's default
*/
#define CLEAR_WARN_COLOR() \
	UE_LOG(LogHAL, SetColor, TEXT("%s"), COLOR_NONE);

#endif
#endif

/**
* Enum that defines how the log times are to be displayed.
*/
namespace ELogTimes
{
	enum Type
	{
		// Do not display log timestamps
		None,

		// Display log timestamps in UTC
		UTC,

		// Display log timestamps in seconds elapsed since GStartTime
		SinceGStartTime,

		// Display log timestamps in local time
		Local
	};
}

class FName;

// An output device.
class X0_API FOutputDevice
{
public:
	FOutputDevice()
		: bSuppressEventTag(false)
		, bAutoEmitLineTerminator(true)
	{}

	FOutputDevice(FOutputDevice&&) = default;
	FOutputDevice(const FOutputDevice&) = default;
	FOutputDevice& operator=(FOutputDevice&&) = default;
	FOutputDevice& operator=(const FOutputDevice&) = default;

	virtual ~FOutputDevice() = default;

	
	// FOutputDevice interface.
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FString& Category) = 0;
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FString& Category, const double Time)
	{
		Serialize(V, Verbosity, Category);
	}

	virtual void Flush()
	{
	}

	/**
	* Closes output device and cleans up. This can't happen in the destructor
	* as we might have to call "delete" which cannot be done for static/ global
	* objects.
	*/
	virtual void TearDown()
	{
	}

	void SetSuppressEventTag(bool bInSuppressEventTag)
	{
		bSuppressEventTag = bInSuppressEventTag;
	}
	inline bool GetSuppressEventTag() const { return bSuppressEventTag; }
	void SetAutoEmitLineTerminator(bool bInAutoEmitLineTerminator)
	{
		bAutoEmitLineTerminator = bInAutoEmitLineTerminator;
	}
	inline bool GetAutoEmitLineTerminator() const { return bAutoEmitLineTerminator; }

	/**
	* Dumps the contents of this output device's buffer to an archive (supported by output device that have a memory buffer)
	* @param Ar Archive to dump the buffer to
	*/
	virtual void Dump(class FArchive& Ar)
	{
	}

	/**
	* @return whether this output device is a memory-only device
	*/
	virtual bool IsMemoryOnly() const
	{
		return false;
	}

	/**
	* @return whether this output device can be used on any thread.
	*/
	virtual bool CanBeUsedOnAnyThread() const
	{
		return false;
	}

	// Simple text printing.
	void Log(const TCHAR* S);
	void Log(ELogVerbosity::Type Verbosity, const TCHAR* S);
	void Log(const FString& Category, ELogVerbosity::Type Verbosity, const TCHAR* Str);
	void Log(const FString& S);
	void Log(const FText& S);
	void Log(ELogVerbosity::Type Verbosity, const FString& S);
	void Log(const FString& Category, ELogVerbosity::Type Verbosity, const FString& S);

private:
	void VARARGS LogfImpl(const TCHAR* Fmt, ...);
	void VARARGS LogfImpl(ELogVerbosity::Type Verbosity, const TCHAR* Fmt, ...);
	void VARARGS CategorizedLogfImpl(const FString& Category, ELogVerbosity::Type Verbosity, const TCHAR* Fmt, ...);

public:
	template <typename FmtType, typename... Types>
	void Logf(const FmtType& Fmt, Types... Args)
	{
		static_assert(TIsArrayOrRefOfType<FmtType, TCHAR>::Value, "Formatting string must be a TCHAR array.");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to FOutputDevice::Logf");

		LogfImpl(Fmt, Args...);
	}

	template <typename FmtType, typename... Types>
	void Logf(ELogVerbosity::Type Verbosity, const FmtType& Fmt, Types... Args)
	{
		static_assert(TIsArrayOrRefOfType<FmtType, TCHAR>::Value, "Formatting string must be a TCHAR array.");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to FOutputDevice::Logf");

		LogfImpl(Verbosity, Fmt, Args...);
	}

	template <typename FmtType, typename... Types>
	void CategorizedLogf(const FName& Category, ELogVerbosity::Type Verbosity, const FmtType& Fmt, Types... Args)
	{
		static_assert(TIsArrayOrRefOfType<FmtType, TCHAR>::Value, "Formatting string must be a TCHAR array.");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to FOutputDevice::CategorizedLogf");

		CategorizedLogfImpl(Category, Verbosity, Fmt, Args...);
	}

protected:
	/** Whether to output the 'Log: ' type front... */
	bool bSuppressEventTag;
	/** Whether to output a line-terminator after each log call... */
	bool bAutoEmitLineTerminator;
};
