#pragma once

#include <vector>

class AActor;

class UWorld
{
public:
	AActor* SpawnActor();
public:
	class FSceneInterface* Scene;
private:
	class ULevel*	CurrentLevel;
};