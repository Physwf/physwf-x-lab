#pragma once

#pragma warning( disable: 4251 )

#include "X0.h"
#include "CoreTypes.h"
#include "ModuleInterface.h"

#include <string>
#include <memory>
#include <map>

class X0_API FModuleManager
{
public:
	~FModuleManager();

	static FModuleManager& Get();

	void AddModule(const std::string& InModuleName);

	IModuleInterface* GetModule(const std::string& InModuleName);

	bool IsModuleLoaded(const std::string&  InModuleName) const;

	IModuleInterface* LoadModule(const std::string& InModuleName);

	IModuleInterface& LoadModuleChecked(const std::string& InModuleName);

	IModuleInterface* LoadModuleWithFailureReason(const std::string& InModuleName);

protected:
	FModuleManager();

private:
	FModuleManager(const FModuleManager&) = delete;
	FModuleManager& operator=(const FModuleManager&) = delete;
protected:
	/**
	 * Information about a single module (may or may not be loaded.)
	 */
	class FModuleInfo
	{
	public:

		/** The original file name of the module, without any suffixes added */
		std::string OriginalFilename;

		/** File name of this module (.dll file name) */
		std::string Filename;

		/** Handle to this module (DLL handle), if it's currently loaded */
		void* Handle;

		/** The module object for this module.  We actually *own* this module, so it's lifetime is controlled by the scope of this shared pointer. */
		std::unique_ptr<IModuleInterface> Module;

		/** True if this module was unloaded at shutdown time, and we never want it to be loaded again */
		bool bWasUnloadedAtShutdown;

		/** Arbitrary number that encodes the load order of this module, so we can shut them down in reverse order. */
		int32 LoadOrder;

		/** static that tracks the current load number. Incremented whenever we add a new module*/
		static int32 CurrentLoadOrder;

	public:

		/** Constructor */
		FModuleInfo()
			: Handle(nullptr)
			, bWasUnloadedAtShutdown(false)
			, LoadOrder(CurrentLoadOrder++)
		{ }

		~FModuleInfo()
		{
		}
	};

	std::map<std::string, std::shared_ptr<FModuleInfo>> Modules;
public:
	void AddModuleToModulesList(const std::string& InModuleName, std::shared_ptr<FModuleInfo>& ModuleInfo);
private:

	std::shared_ptr<FModuleInfo> FindModule(const std::string& InModuleName);
	inline std::shared_ptr<FModuleInfo> FindModule(const std::string& InModuleName) const
	{
		return const_cast<FModuleManager*>(this)->FindModule(InModuleName);
	}
	void FindModulePaths(const char *NamePattern, std::map<std::string, std::string> &OutModulePaths, bool bCanUseCache = true) const;

	mutable std::map<std::string, std::string> ModulePathsCache;
};

typedef IModuleInterface* (*FInitializeModuleFunctionPtr)(void);