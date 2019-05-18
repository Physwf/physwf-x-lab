#pragma once

#include "Modules/ModuleInterface.h"
#include "RHIDefinitions.h"

class X4_API XDynamicRHI
{
public:
	virtual ~XDynamicRHI() {}

	virtual void Init() = 0;

	virtual void PostInit() {};

	virtual void Shutdown() = 0;

};

class X4_API IDynamicRHIModule : public IModuleInterface
{
	virtual bool IsSupported() = 0;

	virtual XDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) = 0;
};