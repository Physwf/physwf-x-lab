#pragma once

#include "X5.h"
#include "Engine.h"


class X5_API UGameEngine : public UEngine
{
public:
	virtual void Init() override;
	virtual void Start() override;

	virtual void Tick() override;

	virtual void RedrawViewports(bool bShouldPresent = true) override;
};