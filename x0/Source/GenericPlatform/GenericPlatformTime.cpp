#include "GenericPlatform/GenericPlatformTime.h"
#include "HAL/PlatformTime.h"
#include "Logging/LogMacros.h"
//#include "Misc/Parse.h"
//#include "Delegates/IDelegateInstance.h"
//#include "Containers/Ticker.h"
//#include "HAL/IConsoleManager.h"
//#include "Stats/Stats.h"

double FGenericPlatformTime::SecondsPerCycle = 0.0;
double FGenericPlatformTime::SecondsPerCycle64 = 0.0;

double FGenericPlatformTime::GetSecondsPerCycle64()
{
	check(SecondsPerCycle64 != 0.0);
	return SecondsPerCycle64;
}

#if PLATFORM_HAS_BSD_TIME 

#include <sched.h>

double FGenericPlatformTime::InitTiming()
{
	// we use gettimeofday() instead of rdtsc, so it's 1000000 "cycles" per second on this faked CPU.
	SecondsPerCycle = 1.0f / 1000000.0f;
	SecondsPerCycle64 = 1.0 / 1000000.0;
	return FPlatformTime::Seconds();
}

void FGenericPlatformTime::SystemTime(int32& Year, int32& Month, int32& DayOfWeek, int32& Day, int32& Hour, int32& Min, int32& Sec, int32& MSec)
{
	// query for calendar time
	struct timeval Time;
	gettimeofday(&Time, NULL);

	// convert it to local time
	struct tm LocalTime;
	localtime_r(&Time.tv_sec, &LocalTime);

	// pull out data/time
	Year = LocalTime.tm_year + 1900;
	Month = LocalTime.tm_mon + 1;
	DayOfWeek = LocalTime.tm_wday;
	Day = LocalTime.tm_mday;
	Hour = LocalTime.tm_hour;
	Min = LocalTime.tm_min;
	Sec = LocalTime.tm_sec;
	MSec = Time.tv_usec / 1000;
}

void FGenericPlatformTime::UtcTime(int32& Year, int32& Month, int32& DayOfWeek, int32& Day, int32& Hour, int32& Min, int32& Sec, int32& MSec)
{
	// query for calendar time
	struct timeval Time;
	gettimeofday(&Time, NULL);

	// convert it to UTC
	struct tm LocalTime;
	gmtime_r(&Time.tv_sec, &LocalTime);

	// pull out data/time
	Year = LocalTime.tm_year + 1900;
	Month = LocalTime.tm_mon + 1;
	DayOfWeek = LocalTime.tm_wday;
	Day = LocalTime.tm_mday;
	Hour = LocalTime.tm_hour;
	Min = LocalTime.tm_min;
	Sec = LocalTime.tm_sec;
	MSec = Time.tv_usec / 1000;
}

#endif

TCHAR* FGenericPlatformTime::StrDate(TCHAR* Dest, SIZE_T DestSize)
{
	int32 Year;
	int32 Month;
	int32 DayOfWeek;
	int32 Day;
	int32 Hour;
	int32 Min;
	int32 Sec;
	int32 MSec;

	FPlatformTime::SystemTime(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
	FCString::Snprintf(Dest, DestSize, TEXT("%02d/%02d/%02d"), Month, Day, Year % 100);
	return Dest;
}

TCHAR* FGenericPlatformTime::StrTime(TCHAR* Dest, SIZE_T DestSize)
{
	int32 Year;
	int32 Month;
	int32 DayOfWeek;
	int32 Day;
	int32 Hour;
	int32 Min;
	int32 Sec;
	int32 MSec;

	FPlatformTime::SystemTime(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
	FCString::Snprintf(Dest, DestSize, TEXT("%02d:%02d:%02d"), Hour, Min, Sec);
	return Dest;
}

const TCHAR* FGenericPlatformTime::StrTimestamp()
{
	static TCHAR Result[1024];
	*Result = 0;
	StrDate(Result, ARRAY_COUNT(Result));
	FCString::Strcat(Result, TEXT(" "));
	StrTime(Result + FCString::Strlen(Result), ARRAY_COUNT(Result) - FCString::Strlen(Result));
	return Result;
}

/**
 * Returns a pretty-string for a time given in seconds. (I.e. "4:31 min", "2:16:30 hours", etc)
 * @param Seconds	Time in seconds
 * @return			Time in a pretty formatted string
 */
FString FGenericPlatformTime::PrettyTime(double Seconds)
{
	if (Seconds < 1.0)
	{
		return FString::Printf(TEXT("%d ms"), FMath::TruncToInt(Seconds * 1000));
	}
	else if (Seconds < 10.0)
	{
		int32 Sec = FMath::TruncToInt(Seconds);
		int32 Ms = FMath::TruncToInt(Seconds * 1000) - Sec * 1000;
		return FString::Printf(TEXT("%d.%02d sec"), Sec, Ms / 10);
	}
	else if (Seconds < 60.0)
	{
		int32 Sec = FMath::TruncToInt(Seconds);
		int32 Ms = FMath::TruncToInt(Seconds * 1000) - Sec * 1000;
		return FString::Printf(TEXT("%d.%d sec"), Sec, Ms / 100);
	}
	else if (Seconds < 60.0*60.0)
	{
		int32 Min = FMath::TruncToInt(Seconds / 60.0);
		int32 Sec = FMath::TruncToInt(Seconds) - Min * 60;
		return FString::Printf(TEXT("%d:%02d min"), Min, Sec);
	}
	else
	{
		int32 Hr = FMath::TruncToInt(Seconds / 3600.0);
		int32 Min = FMath::TruncToInt((Seconds - Hr * 3600) / 60.0);
		int32 Sec = FMath::TruncToInt(Seconds - Hr * 3600 - Min * 60);
		return FString::Printf(TEXT("%d:%02d:%02d hours"), Hr, Min, Sec);
	}
}
