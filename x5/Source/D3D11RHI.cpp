#include "Windows/D3D11RHIPrivate.h"

XD3D11DynamicRHI::XD3D11DynamicRHI(IDXGIFactory1* InDXGIFactory1, D3D_FEATURE_LEVEL InFeatureLevel, int InChoseAdapter, const DXGI_ADAPTER_DESC& InChosenDescription):
	DXGIFactory(InDXGIFactory1),
	FeatureLevel(InFeatureLevel),
	ChoseAdapter(InChoseAdapter),
	ChosenDescription(InChosenDescription)
{

}

XD3D11DynamicRHI::~XD3D11DynamicRHI()
{

}

void XD3D11DynamicRHI::InitD3DDevice()
{

}

