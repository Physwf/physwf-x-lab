#pragma once

#include "CoreMinimal.h"
#include "DynamicRHI.h"

#include "Windows/D3D11RHIBasePrivate.h"

#include "D3D11Util.h"

class XD3D11DynamicRHI
{
public:
	XD3D11DynamicRHI(IDXGIFactory1* InDXGIFactory1, D3D_FEATURE_LEVEL InFeatureLevel, int InChoseAdapter, const DXGI_ADAPTER_DESC& ChosenDescription);

	virtual ~XD3D11DynamicRHI();

	void InitD3DDevice();
protected:
	std::shared_ptr<IDXGIFactory1> DXGIFactory;
	D3D_FEATURE_LEVEL FeatureLevel;
	int ChoseAdapter;
	DXGI_ADAPTER_DESC ChosenDescription;
};

struct XD3D11Adapter
{
	int32 AdapterIndex;

	D3D_FEATURE_LEVEL MaxSupportFeatureLevel;

	XD3D11Adapter(int32 InAdapterIndex = -1, D3D_FEATURE_LEVEL InMaxSupportFeatureLevel = (D3D_FEATURE_LEVEL)0)
		:AdapterIndex(InAdapterIndex),
		MaxSupportFeatureLevel(InMaxSupportFeatureLevel)
	{

	}

	bool IsValid() const
	{
		return MaxSupportFeatureLevel != (D3D_FEATURE_LEVEL)0 && AdapterIndex >= 0;
	}
};

class XD3DDynamicRHIModule : public IDynamicRHIModule
{
	virtual void StartupModule() override;

	virtual bool IsSupported() override;
	virtual XDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) override;
private:
	XD3D11Adapter ChosenAdapter;

	DXGI_ADAPTER_DESC ChosenDescription;

	void FindAdapter();
};


