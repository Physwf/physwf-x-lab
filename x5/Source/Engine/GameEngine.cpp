#include "GameEngine.h"
#include "GameViewportClient.h"
#include "GameInstance.h"

void UGameEngine::Init()
{
	UEngine::Init();

	GameInstance = new UGameInstance(this);
	GameInstance->InitializeStandalone();

	GameViewport = new UGameViewportClient();
}

void UGameEngine::Start()
{
	GameInstance->StartGameInstance();
}

void UGameEngine::Tick()
{
	RedrawViewports();
}

void UGameEngine::RedrawViewports(bool bShouldPresent /*= true*/)
{

}

