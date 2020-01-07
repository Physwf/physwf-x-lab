#pragma once

#include "Engine.h"

class UGameEngine : public UEngine
{
public:
	virtual void Init() override;

	virtual void Tick() override;

	virtual void RedrawViewports(bool bShouldPresent = true) override;
};