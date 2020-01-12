#pragma once

#pragma warning( disable: 4251 )

#include "X5.h"

#include <vector>

class UWorld;

struct FWorldContext
{
	X5_API void SetCurrentWorld(UWorld *World);

	inline UWorld* World() const
	{
		return ThisCurrentWorld;
	}
private:
	UWorld * ThisCurrentWorld;
};

class X5_API UEngine
{
protected:
	class UGameInstance * GameInstance;
public:
	virtual void Init();
	virtual void Start();

	virtual void Tick();

	virtual void RedrawViewports(bool bShouldPresent = true) { }

	FWorldContext& CreateNewWorldContext();

	virtual bool Browse(FWorldContext& WorldContext);
	virtual bool LoadMap(FWorldContext& WorldContext);
public:
	class UGameViewportClient* GameViewport;
protected:
	std::vector<FWorldContext*> WorldList;
};