#pragma once

#include "RHIDefinitions.h"

class XDynamicRHI
{
public:
	virtual ~XDynamicRHI() {}

	virtual void Init() = 0;

	virtual void PostInit() {};

	virtual void Shutdown() = 0;

};

class IDynamicRHIModule : public IModuleInterface
{
	virtual bool IsSupported() = 0;

	virtual XDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) = 0;
};