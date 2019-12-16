#include "CoreTypes.h"
#include "LaunchEngineLoop.h"

FEngineLoop	GEngineLoop;
bool GIsRequestingExit;

int32 EnginePreInit(/*const TCHAR* CmdLine*/)
{
	int32 ErrorLevel = GEngineLoop.PreInit();

	return(ErrorLevel);
}

int32 EngineInit()
{
	int32 ErrorLevel = GEngineLoop.Init();

	return(ErrorLevel);
}


void EngineTick(void)
{
	GEngineLoop.Tick();
}


void EngineExit(void)
{
	// Make sure this is set
	//GIsRequestingExit = true;

	GEngineLoop.Exit();
}


int32 GuardedMain()
{
	EnginePreInit();

	EngineInit();

	while (!GIsRequestingExit)
	{
		EngineTick();
	}

	EngineExit();

	return 0;
}