#pragma once

#include "CoreTypes.h"
#include "HAL/PlatformMisc.h"
#include "Templates/AndOrNot.h"
#include "Templates/EnableIf.h"
#include "Templates/IsArrayOrRefOfType.h"
#include "Templates/IsValidVariadicFunctionArg.h"
#include "Misc/VarArgs.h"

namespace ELogVerbosity
{
	enum Type : uint8;
}

extern "C" DLLEXPORT void PrintScriptCallStack();

struct X0_API XDebug
{
	static void VARARGS AssertFailed(const ANSICHAR* Expr, const ANSICHAR* File, int32 Line, const TCHAR* Format* = TEXT(""), ...);

	static bool HasAsserted();

	static bool IsEnsuring();

	static void DumpStatckTraceToLog();

#if DO_CHECK || DO_GUARD_SLOW
private:
	static void VARARGS LogAssertFailedMessageImpl(const ANSICHAR* Expr, const ANSICHAR* File, int32 Line, const TCHAR* Fmt, ...);

public:
	template<typename FmtType, typename...Types>
	static void LogAssertFailedMessage(const ANSICHAR* Expr, const ANSICHAR* File, int32 Line, const FmtType& Fmt, Types ...Args)
	{
		static_assert(TIsArrayOrRefType<FmtType, TCHAR>::Value, "Formatting string must be TCHAR array!");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to XDebug::LogAssertFailedMessage");

		LogAssertFailedMessageImpl(Expr, File, Line, Fmt, Args...);
	}

	static void EnsureFailed(const ANSICHAR* Expr, const ANSICHAR* File, int32 Line, const TCHAR* Msg);

private:
	static bool VARARGS OptionallyLogFormattedEnsureMassageReturningFalseImpl(bool bLog,const ANSICHAR* Expr, const ANSICHAR* File, int32 Line, const TCHAR* FormattedMsg,...);

public:

	template<typename FmtType,typename ...Types>
	static FORCEINLINE typename TEnableIf<TIsArrayOrRefType<FmtType, TCHAR>::Value, bool>::Type OptionallyLogFormattedEnsureMessageReturningFalse(bool bLog, const ANSICHAR* Expr, const ANSICHAR* File, int32 Line, const FmtType& FormattedMsg, Types...Args)
	{
		static_assert(TIsArrayOrRefType<FmtType, TCHAR>::Value, "Formatting string must be a TCHAR array.");
		static_assert(TAnd<TIsValidVariadicFunctionArg<Types>...>::Value, "Invalid argument(s) passed to ensureMsgF");

		return OptionallyLogFormattedEnsureMassageReturningFalseImpl(bLog, Expr, File, Line, FormattedMsg, Args...);
	}

	static void LogFormattedMessageWithCallStack(const XName& LogName, const ANSICHAR* File, int32 Line, const TCHAR* Heading, const TCHAR* Message, ELogVerbosity::Type Verbosity);
};