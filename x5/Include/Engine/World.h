#pragma once

#include <vector>

class AActor;
class UGameInstance;

class UWorld
{
public:
	AActor* SpawnActor();

	void InitWorld();
	void InitializeNewWorld();
	static UWorld* CreateWorld();
	void DestroyWorld();

	inline void SetGameInstance(UGameInstance* NewGI)
	{
		OwningGameInstance = NewGI;
	}
public:
	class FSceneInterface* Scene;
private:
	class ULevel*				CurrentLevel;
	class UGameInstance*		OwningGameInstance;
};