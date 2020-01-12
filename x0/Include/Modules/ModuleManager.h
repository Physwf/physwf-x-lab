#pragma once

#include "X0.h"
#include "ModuleInterface.h"

class X0_API FModuleManager
{
public:
	~FModuleManager();

	static FModuleManager& Get();

	IModuleInterface* GetModule(const char* InModuleName);

	bool IsModuleLoaded(const char*  InModuleName) const;

	IModuleInterface* LoadModule(const char* InModuleName);

	IModuleInterface& LoadModuleChecked(const char* InModuleName);

	IModuleInterface* LoadModuleWithFailureReason(const char* InModuleName);

protected:
	FModuleManager();

private:
	FModuleManager(const FModuleManager&) = delete;
	FModuleManager& operator=(const FModuleManager&) = delete;
};