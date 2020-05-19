#include "Misc/OutputDevice.h"
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"
//#include "Internationalization/Text.h"
#include "Logging/LogScopedCategoryAndVerbosityOverride.h"
#include "Misc/OutputDeviceHelper.h"
#include "Misc/VarargsHelper.h"

#include "CoreGlobals.h"

DEFINE_LOG_CATEGORY(LogOutputDevice);

void FOutputDevice::Log(ELogVerbosity::Type Verbosity, const TCHAR* Str)
{
	Serialize(Str, Verbosity, TEXT("None"));
}
void FOutputDevice::Log(ELogVerbosity::Type Verbosity, const FString& S)
{
	Serialize(*S, Verbosity, TEXT("None"));
}
void FOutputDevice::Log(const class FString& Category, ELogVerbosity::Type Verbosity, const TCHAR* Str)
{
	Serialize(Str, Verbosity, Category);
}
void FOutputDevice::Log(const class FString& Category, ELogVerbosity::Type Verbosity, const FString& S)
{
	Serialize(*S, Verbosity, Category);
}
void FOutputDevice::Log(const TCHAR* Str)
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	Serialize(Str, TLS->Verbosity, TLS->Category);
}
void FOutputDevice::Log(const FString& S)
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	Serialize(*S, TLS->Verbosity, TLS->Category);
}

/*-----------------------------------------------------------------------------
	Formatted printing and messages.
-----------------------------------------------------------------------------*/

void FOutputDevice::CategorizedLogfImpl(const FString& Category, ELogVerbosity::Type Verbosity, const TCHAR* Fmt, ...)
{
	GROWABLE_LOGF(Serialize(Buffer, Verbosity, Category))
}
void FOutputDevice::LogfImpl(ELogVerbosity::Type Verbosity, const TCHAR* Fmt, ...)
{
	// call serialize with the final buffer
	GROWABLE_LOGF(Serialize(Buffer, Verbosity, TEXT("None")))
}
void FOutputDevice::LogfImpl(const TCHAR* Fmt, ...)
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	GROWABLE_LOGF(Serialize(Buffer, TLS->Verbosity, TLS->Category))
}



/** Critical errors. */
X0_API FOutputDeviceError* GError = NULL;