#include "D3D11RHIPrivate.h"
#include "Templates/RefCounting.h"

#include <delayimp.h>

static bool IsDelayLoadException(PEXCEPTION_POINTERS ExceptionPointers)
{
#if WINVER > 0x502
	switch (ExceptionPointers->ExceptionRecord->ExceptionCode)
	{
	case VcppException(ERROR_SEVERITY_ERROR,ERROR_MOD_NOT_FOUND):
	case VcppException(ERROR_SEVERITY_ERROR,ERROR_PROC_NOT_FOUND):
		return EXCEPTION_EXECUTE_HANDLER;
	default:
		return EXCEPTION_CONTINUE_SEARCH;
	}
#else
	return EXCEPTION_EXECUTE_HANDLER;
#endif
}

static void SafeCreateDXGIFactory(IDXGIFactory1** DXGIFactory1)
{
	__try
	{
		CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)DXGIFactory1);
	}
	__except (IsDelayLoadException(GetExceptionInformation()))
	{

	}
}

static D3D_FEATURE_LEVEL GetAllowedD3DFeatureLevel()
{
	D3D_FEATURE_LEVEL AllowedFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	return AllowedFeatureLevel;
}

static bool SafeTestD3D11CreateDevice(IDXGIAdapter* Adapter, D3D_FEATURE_LEVEL MaxFeatureLevel, D3D_FEATURE_LEVEL* OutFeatureLevel)
{
	ID3D11Device* D3DDevice = NULL;
	ID3D11DeviceContext* D3DDeviceContext = NULL;
	uint32 DeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;

	DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL RequesetedFeatureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	int32 FirstAllowedFeatureLevel = 0;
	int32 NumAllowedFeatureLevels = sizeof(RequesetedFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	while (FirstAllowedFeatureLevel < NumAllowedFeatureLevels)
	{
		if (RequesetedFeatureLevels[FirstAllowedFeatureLevel] == MaxFeatureLevel)
		{
			break;
		}
		FirstAllowedFeatureLevel++;
	}
	NumAllowedFeatureLevels -= FirstAllowedFeatureLevel;

	if (NumAllowedFeatureLevels == 0)
	{
		return false;
	}

	__try
	{
		if (SUCCEEDED(D3D11CreateDevice(Adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			DeviceFlags,
			&RequesetedFeatureLevels[FirstAllowedFeatureLevel],
			NumAllowedFeatureLevels,
			D3D11_SDK_VERSION,
			&D3DDevice,
			OutFeatureLevel,
			&D3DDeviceContext
		)))
		{
			D3DDevice->Release();
			D3DDeviceContext->Release();
			return true;
		}
	}
	__except (IsDelayLoadException(GetExceptionInformation()))
	{

	}

	return false;
}

void XD3DDynamicRHIModule::StartupModule()
{

}

XDynamicRHI* XD3DDynamicRHIModule::CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel /*= ERHIFeatureLevel::Num*/)
{
	return nullptr;
}

bool XD3DDynamicRHIModule::IsSupported()
{
	return false;
}

static uint32 CountAdapterOutputs(TRefCountPtr<IDXGIAdapter>& Adapter)
{
	uint32 OutputCount = 0;
	for (;;)
	{
		TRefCountPtr<IDXGIOutput> Output;
		HRESULT hr = Adapter->EnumOutputs(OutputCount, Output.GetInitReference());
		if (FAILED(hr))
		{
			break;
		}
		OutputCount++;
	}
	return OutputCount;
}

void XD3DDynamicRHIModule::FindAdapter()
{
	IDXGIFactory1* DXGIFactory1 = nullptr;
	SafeCreateDXGIFactory(&DXGIFactory1);
	if (!DXGIFactory1)
	{
		return;
	}

	TRefCountPtr<IDXGIAdapter> TempAdapter;
	D3D_FEATURE_LEVEL MaxAllowedFeatureLevel = GetAllowedD3DFeatureLevel();

	XD3D11Adapter FirstWithoutIntegratedAdapter;
	XD3D11Adapter FirstAdapter;

	std::vector<DXGI_ADAPTER_DESC> AdapterDescription;

	for (uint32 AdapterIndex = 0; DXGIFactory1->EnumAdapters(AdapterIndex, TempAdapter.GetInitReference()) != DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
	{
		DXGI_ADAPTER_DESC Zero = { 0 };
		AdapterDescription.push_back(Zero);
		DXGI_ADAPTER_DESC& AdpaterDesc = AdapterDescription[AdapterDescription.size() - 1];

		if (TempAdapter)
		{
			D3D_FEATURE_LEVEL ActureFeatureLevel = (D3D_FEATURE_LEVEL)0;

			if (SafeTestD3D11CreateDevice(TempAdapter, MaxAllowedFeatureLevel, &ActureFeatureLevel))
			{
				VERIFYD3D11RESULT(TempAdapter->GetDesc(&AdpaterDesc));
				uint32 OutputCount = CountAdapterOutputs(TempAdapter);
			}
		}
	}
}
