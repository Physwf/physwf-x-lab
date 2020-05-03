// Core includes.
#include "Misc/Paths.h"
//#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFile.h"
//#include "Misc/Parse.h"
#include "Misc/ScopeLock.h"
//#include "Misc/CommandLine.h"
//#include "Internationalization/Text.h"
//#include "Internationalization/Internationalization.h"
#include "Misc/Guid.h"
//#include "Misc/ConfigCacheIni.h"
#include "Misc/App.h"
//#include "Misc/DataDrivenPlatformInfoRegistry.h"
//#include "Misc/EngineVersion.h"


FString FPaths::LaunchDir()
{
	return FString(FPlatformMisc::LaunchDir());
}

FString FPaths::EngineDir()
{
	return FString(FPlatformMisc::EngineDir());
}

FString FPaths::EngineContentDir()
{
	return FPaths::EngineDir() + TEXT("Content/");
}

FString FPaths::EngineConfigDir()
{
	return FPaths::EngineDir() + TEXT("Config/");
}

FString FPaths::EngineIntermediateDir()
{
	return FPaths::EngineDir() + TEXT("Intermediate/");
}

FString FPaths::EngineSavedDir()
{
	return EngineUserDir() + TEXT("Saved/");
}

FString FPaths::EnginePluginsDir()
{
	return FPaths::EngineDir() + TEXT("Plugins/");
}

FString FPaths::EnterpriseDir()
{
	return FPaths::RootDir() + TEXT("Enterprise/");
}

FString FPaths::EnterprisePluginsDir()
{
	return EnterpriseDir() + TEXT("Plugins/");
}

FString FPaths::EnterpriseFeaturePackDir()
{
	return FPaths::EnterpriseDir() + TEXT("FeaturePacks/");
}

FString FPaths::RootDir()
{
	return FString(FPlatformMisc::RootDir());
}

FString FPaths::ProjectDir()
{
	return FString(FPlatformMisc::ProjectDir());
}