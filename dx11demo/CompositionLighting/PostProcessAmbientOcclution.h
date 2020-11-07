#pragma once

#include "D3D11RHI.h"
#include "UnrealMath.h"

class RCPassPostProcessAmbientOcclusionSetup
{
public:
	void Init();
	void Process();
private:
	bool IsInitialPass() const;
public:
	ID3DBlob* PSBytecode;
	ID3D11PixelShader* PS;
	ID3D11ShaderResourceView* GBufferASRV;
	ID3D11SamplerState* GBufferASamplerState;
	ID3D11ShaderResourceView* SceneDepthSRV;
	ID3D11SamplerState* SceneDepthSamplerState;
	ID3D11RasterizerState* RasterState;
	ID3D11BlendState* BlendState;

	ID3D11Texture2D* Input0;
	ID3D11Texture2D* Input1;

	IntPoint OutputExtent;
	ID3D11Texture2D* Output;
	ID3D11RenderTargetView* OutputRTV;

	std::map<std::string, ParameterAllocation> PSParams;
};

class RCPassPostProcessAmbientOcclusion
{
public:
	void Init();
	void Process();
public:
	ID3DBlob * PSBytecode;
	ID3D11PixelShader* PS;
	ID3D11RasterizerState* RasterState;
	ID3D11BlendState* BlendState;

	ID3D11Texture2D* Input0;
	ID3D11Texture2D* Input1;
	ID3D11Texture2D* Input2;
	ID3D11Texture2D* Input3;

	ID3D11Texture2D* Output;
	ID3D11RenderTargetView* OutputRTV;

	std::map<std::string, ParameterAllocation> PSParams;
};

class RCPassPostProcessBasePassAO 
{
public:
	void Init();
	void Process();
public:
	ID3DBlob * PSBytecode;
	ID3D11PixelShader* PS;
	ID3D11RasterizerState* RasterState;
	ID3D11BlendState* BlendState;

	ID3D11Texture2D* Output;
	ID3D11RenderTargetView* OutputRTV;

	std::map<std::string, ParameterAllocation> PSParams;
};