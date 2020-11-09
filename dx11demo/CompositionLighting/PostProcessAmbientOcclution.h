#pragma once

#include "D3D11RHI.h"
#include "UnrealMath.h"
#include "Scene.h"

class RCPassPostProcessAmbientOcclusionSetup
{
public:
	void Init();
	void Process(ViewInfo& View);
private:
	bool IsInitialPass() const;
public:
	ID3DBlob* PSBytecode;
	ID3D11PixelShader* PS;
	ID3D11ShaderResourceView* GBufferASRV;
	ID3D11SamplerState* GBufferASamplerState;
	ID3D11ShaderResourceView* GBufferBSRV;
	ID3D11SamplerState* GBufferBSamplerState;
	ID3D11ShaderResourceView* SceneDepthSRV;
	ID3D11SamplerState* SceneDepthSamplerState;
	ID3D11RasterizerState* RasterState;
	ID3D11BlendState* BlendState;

	ID3D11Texture2D* Inputs[2];

	IntPoint OutputExtent;
	ID3D11Texture2D* Output;
	ID3D11RenderTargetView* OutputRTV;

	std::map<std::string, ParameterAllocation> PSParams;
};

class RCPassPostProcessAmbientOcclusion
{
public:
	void Init(bool InAOSetupAsInput=true);
	void Process(ViewInfo& View);
private:
	void ProcessPS(ID3D11RenderTargetView* DestRenderTarget, ViewInfo& View, const IntRect& ViewRect, const IntPoint& TexSize, int32 ShaderQuality, bool bDoUpsample);
public:
	ID3DBlob * PSBytecode;
	ID3D11PixelShader* PS;
	ID3D11RasterizerState* RasterState;
	ID3D11BlendState* BlendState;
	ID3D11ShaderResourceView* RandomNormalSRV;
	ID3D11SamplerState* RandomNormalSampler;
	ID3D11ShaderResourceView* GBufferASRV;
	ID3D11SamplerState* GBufferASamplerState;
	ID3D11ShaderResourceView* GBufferBSRV;
	ID3D11SamplerState* GBufferBSamplerState;
	ID3D11ShaderResourceView* SceneDepthSRV;
	ID3D11SamplerState* SceneDepthSamplerState;
	ID3D11ShaderResourceView* PostprocessInput0;
	ID3D11SamplerState* PostprocessInput0Sampler;
	ID3D11ShaderResourceView* PostprocessInput1;
	ID3D11SamplerState* PostprocessInput1Sampler;
	ID3D11ShaderResourceView* PostprocessInput2;
	ID3D11SamplerState* PostprocessInput2Sampler;
	ID3D11ShaderResourceView* PostprocessInput3;
	ID3D11SamplerState* PostprocessInput3Sampler;

	ID3D11Texture2D* Inputs[4];

	ID3D11Texture2D* Output;
	IntPoint OutputExtent;
	ID3D11RenderTargetView* OutputRTV;

	std::map<std::string, ParameterAllocation> PSParams;
private:
	bool bAOSetupAsInput;
};

class RCPassPostProcessBasePassAO 
{
public:
	void Init();
	void Process(ViewInfo& View);
public:
	ID3DBlob * PSBytecode;
	ID3D11PixelShader* PS;
	ID3D11RasterizerState* RasterState;
	ID3D11BlendState* BlendState;

	ID3D11Texture2D* Output;
	ID3D11RenderTargetView* OutputRTV;

	std::map<std::string, ParameterAllocation> PSParams;
};