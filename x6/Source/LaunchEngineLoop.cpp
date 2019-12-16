#include "LaunchEngineLoop.h"

#include "Engine.h"

JEngine GEngine;

FEngineLoop::FEngineLoop()
{

}

int32 FEngineLoop::PreInit()
{
	return 0;
}

int32 FEngineLoop::Init()
{
	GEngine.Init();
	return 0;
}

void FEngineLoop::Exit()
{

}

void FEngineLoop::Tick()
{
	GEngine.Tick();
}

