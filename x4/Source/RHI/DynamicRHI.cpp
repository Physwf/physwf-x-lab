#include "CoreMinimal.h"
//#include "Misc/MessageDialog.h"
//#include "Misc/OutputDeviceRedirector.h"
//#include "HAL/IConsoleManager.h"
//#include "Misc/App.h"
#include "RHI/RHI.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformDriver.h"
#include "RHI/PipelineStateCache.h"

#ifndef PLATFORM_ALLOW_NULL_RHI
#define PLATFORM_ALLOW_NULL_RHI		0
#endif

// Globals.
FDynamicRHI* GDynamicRHI = NULL;

void InitNullRHI()
{
	
}

void RHIInit(bool bHasEditorToken)
{
	if (!GDynamicRHI)
	{
		GRHICommandList.LatchBypass();

		GDynamicRHI = PlatformCreateDynamicRHI();

		if (GDynamicRHI)
		{
			GDynamicRHI->Init();
		}

		GRHICommandList.GetImmediateCommandList().SetContext(GDynamicRHI->RHIGetDefaultContext());
		GRHICommandList.GetImmediateAsyncComputeCommandList().SetComputeContext(GDynamicRHI->RHIGetDefaultAsyncComputeContext());

		if (bHasEditorToken && GMaxRHIFeatureLevel < ERHIFeatureLevel::SM5)
		{

		}
	}
}

void RHIPostInit(const TArray<uint32>& InPixelFormatByteWidth)
{
	check(GDynamicRHI);
	GDynamicRHI->InitPixelFormatInfo(InPixelFormatByteWidth);
	GDynamicRHI->PostInit();
}

void RHIExit()
{
	if (GDynamicRHI != NULL)
	{
		PipelineStateCache::Shutdown();

		GDynamicRHI->Shutdown();
		delete GDynamicRHI;
		GDynamicRHI = NULL;
	}
}

void FDynamicRHI::EnableIdealGPUCaptureOptions(bool bEnabled)
{
#if 0
	static IConsoleVariable* RHICmdBypassVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.rhicmdbypass"));
	static IConsoleVariable* ShowMaterialDrawEventVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShowMaterialDrawEvents"));
	static IConsoleObject* RHIThreadEnableObj = IConsoleManager::Get().FindConsoleObject(TEXT("r.RHIThread.Enable"));
	static IConsoleCommand* RHIThreadEnableCommand = RHIThreadEnableObj ? RHIThreadEnableObj->AsCommand() : nullptr;

	const bool bShouldEnableDrawEvents = bEnabled;
	const bool bShouldEnableMaterialDrawEvents = bEnabled;
	const bool bShouldEnableRHIThread = !bEnabled;
	const bool bShouldRHICmdBypass = bEnabled;

	const bool bDrawEvents = GetEmitDrawEvents() != 0;
	const bool bMaterialDrawEvents = ShowMaterialDrawEventVar ? ShowMaterialDrawEventVar->GetInt() != 0 : false;
	const bool bRHIThread = IsRunningRHIInSeparateThread();
	const bool bRHIBypass = RHICmdBypassVar ? RHICmdBypassVar->GetInt() != 0 : false;

	UE_LOG(LogRHI, Display, TEXT("Setting GPU Capture Options: %i"), bEnabled ? 1 : 0);
	if (bShouldEnableDrawEvents != bDrawEvents)
	{
		UE_LOG(LogRHI, Display, TEXT("Toggling draw events: %i"), bShouldEnableDrawEvents ? 1 : 0);
		SetEmitDrawEvents(bShouldEnableDrawEvents);
	}
	if (bShouldEnableMaterialDrawEvents != bMaterialDrawEvents && ShowMaterialDrawEventVar)
	{
		UE_LOG(LogRHI, Display, TEXT("Toggling showmaterialdrawevents: %i"), bShouldEnableDrawEvents ? 1 : 0);
		ShowMaterialDrawEventVar->Set(bShouldEnableDrawEvents ? -1 : 0);
	}
	if (bRHIThread != bShouldEnableRHIThread && RHIThreadEnableCommand)
	{
		UE_LOG(LogRHI, Display, TEXT("Toggling rhi thread: %i"), bShouldEnableRHIThread ? 1 : 0);
		TArray<FString> Args;
		Args.Add(FString::Printf(TEXT("%i"), bShouldEnableRHIThread ? 1 : 0));
		RHIThreadEnableCommand->Execute(Args, nullptr, *GLog);
	}
	if (bRHIBypass != bShouldRHICmdBypass && RHICmdBypassVar)
	{
		UE_LOG(LogRHI, Display, TEXT("Toggling rhi bypass: %i"), bEnabled ? 1 : 0);
		RHICmdBypassVar->Set(bShouldRHICmdBypass ? 1 : 0, ECVF_SetByConsole);
	}
#endif
}