#include "LaunchEngineLoop.h"

#include "GameEngine.h"

UEngine* GEngine;

FEngineLoop::FEngineLoop()
{

}

int32 FEngineLoop::PreInit()
{
	return 0;
}

int32 FEngineLoop::Init()
{
	GEngine = new UGameEngine();
	GEngine->Init();
	GEngine->Start();
	return 0;
}

void FEngineLoop::Exit()
{

}

void FEngineLoop::Tick()
{
	GEngine->Tick();
}

