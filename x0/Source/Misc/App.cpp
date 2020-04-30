#include "Misc/App.h"
//#include "HAL/FileManager.h"
//#include "Misc/Paths.h"
//#include "Misc/ConfigCacheIni.h"
//#include "Runtime/Launch/Resources/Version.h"
//#include "HAL/LowLevelMemTracker.h"
//#include "BuildSettings.h"

FGuid FApp::InstanceId = FGuid::NewGuid();
FGuid FApp::SessionId = FGuid::NewGuid();
FString FApp::SessionName = FString();
FString FApp::SessionOwner = FString();
TArray<FString> FApp::SessionUsers = TArray<FString>();
bool FApp::Standalone = true;
bool FApp::bIsBenchmarking = false;
bool FApp::bUseFixedSeed = false;
bool FApp::bUseFixedTimeStep = false;
double FApp::FixedDeltaTime = 1 / 30.0;
double FApp::CurrentTime = 0.0;
double FApp::LastTime = 0.0;
double FApp::DeltaTime = 1 / 30.0;
double FApp::IdleTime = 0.0;
double FApp::IdleTimeOvershoot = 0.0;
//FTimecode FApp::Timecode = FTimecode();
float FApp::VolumeMultiplier = 1.0f;
float FApp::UnfocusedVolumeMultiplier = 0.0f;
bool FApp::bUseVRFocus = false;
bool FApp::bHasVRFocus = false;

bool FApp::IsRunningDebug()
{
// 	static FString RunConfig;
// 	static const bool bHasRunConfig = FParse::Value(FCommandLine::Get(), TEXT("RunConfig="), RunConfig);
// 	static const bool bRunningDebug = FParse::Param(FCommandLine::Get(), TEXT("debug"))
// 		|| (bHasRunConfig && RunConfig.StartsWith(TEXT("Debug")));
// 	return bRunningDebug;
	return false;
}

void FApp::InitializeSession()
{

}
