#include "GameEngine.h"
#include "GameViewportClient.h"

void UGameEngine::Init()
{
	GameViewport = new UGameViewportClient();
}

void UGameEngine::Tick()
{
	RedrawViewports();
}

void UGameEngine::RedrawViewports(bool bShouldPresent /*= true*/)
{

}

