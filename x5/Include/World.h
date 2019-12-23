#pragma once

#include <vector>

class Actor;

class UWorld
{
public:
	AActor* SpawnActor();
private:
	class ULevel*	CurrentLevel;
}; 