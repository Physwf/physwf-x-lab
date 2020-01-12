#include "GameInstance.h"
#include "Engine.h"
#include "World.h"

void UGameInstance::InitializeStandalone()
{
	WorldContext = &GetEngine()->CreateNewWorldContext();

	UWorld* DummyWorld = UWorld::CreateWorld();
	DummyWorld->SetGameInstance(this);
	WorldContext->SetCurrentWorld(DummyWorld);

	Init();
}

void UGameInstance::Init()
{

}

void UGameInstance::StartGameInstance()
{
	UEngine* const Engine = GetEngine();
	Engine->Browse(*WorldContext);
}

class UWorld* UGameInstance::GetWorld() const
{
	return WorldContext ? WorldContext->World() : nullptr;
}

class UEngine* UGameInstance::GetEngine() const
{
	return Outer;
}

