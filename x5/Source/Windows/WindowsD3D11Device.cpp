#include "D3D11RHIPrivate.h"

static bool IsDelayLoadException(PEXCEPTION_POINTERS ExceptionPointers)
{

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
}

void XD3DDynamicRHIModule::StartupModule()
{

}

XDynamicRHI* XD3DDynamicRHIModule::CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel /*= ERHIFeatureLevel::Num*/)
{

}

bool XD3DDynamicRHIModule::IsSupported()
{

}

void XD3DDynamicRHIModule::FindAdapter()
{
	IDXGIFactory1* DXGIFactory1 = nullptr;
	SafeCreateDXGIFactory(&DXGIFactory1);
	if (!DXGIFactory1)
	{
		return;
	}

	std::shared_ptr<IDXGIAdapter> TempAdapter;
	D3D_FEATURE_LEVEL MaxAllowedFeatureLevel = GetAllowedD3DFeatureLevel();

	XD3D11Adapter FirstWithoutIntegratedAdapter;
	XD3D11Adapter FirstAdapter;

	std::vector<DXGI_ADAPTER_DESC> AdapterDescription;

	for (uint32 AdapterIndex = 0; DXGIFactory1->EnumAdapters(AdapterIndex, &TempAdapter.get()) != DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
	{
		DXGI_ADAPTER_DESC Zero = { 0 };
		AdapterDescription.push_back(Zero);
		DXGI_ADAPTER_DESC& AdpaterDesc = AdapterDescription[AdapterDescription.size() - 1];

		if (TempAdapter.get())
		{
			D3D_FEATURE_LEVEL ActureFeatureLevel = (D3D_FEATURE_LEVEL)0;

		}
	}
}
