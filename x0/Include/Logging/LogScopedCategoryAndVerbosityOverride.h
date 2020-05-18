#pragma once

#include "CoreTypes.h"
//#include "UObject/NameTypes.h"
#include "Containers/UnrealString.h"

#include "X0.h"
/**
* Helper class that uses thread local storage to set up the default category and verbosity for the low level logging functions.
* This is what allow a UE_LOG(LogHAL, Log, TEXT("...")); within a UE_LOG statement to know what the category and verbosity is.
* When one of these goes out of scope, it restores the previous values.
**/
class X0_API FLogScopedCategoryAndVerbosityOverride
{
public:
	/** STructure to aggregate a category and verbosity pair **/
	struct FOverride
	{
		ELogVerbosity::Type Verbosity;
		FString Category;
		FOverride()
			: Verbosity(ELogVerbosity::Log)
		{
		}
		FOverride(FString InCategory, ELogVerbosity::Type InVerbosity)
			: Verbosity(InVerbosity)
			, Category(InCategory)
		{
		}
		void operator=(const FOverride& Other)
		{
			Verbosity = Other.Verbosity;
			Category = Other.Category;
		}
	};
private:
	/** Backup of the category, verbosity pairs that was present when we were constructed **/
	FOverride Backup;
public:
	/** Back up the existing category and varbosity pair, then sets them.*/
	FLogScopedCategoryAndVerbosityOverride(FString Category, ELogVerbosity::Type Verbosity);

	/** Restore the category and verbosity overrides to the previous value.*/
	~FLogScopedCategoryAndVerbosityOverride();

	/** Manages a TLS slot with the current overrides for category and verbosity. **/
	static FOverride* GetTLSCurrent();
};

typedef FLogScopedCategoryAndVerbosityOverride FScopedCategoryAndVerbosityOverride;