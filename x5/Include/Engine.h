#pragma once

#include "X5.h"

class X5_API UEngine
{
public:
	virtual void Init();

	virtual void Tick();

	virtual void RedrawViewports(bool bShouldPresent = true) { }

public:
	class UGameViewportClient* GameViewport;
};