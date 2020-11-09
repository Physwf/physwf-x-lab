#include "PostProcessAmbientOcclution.h"
#include "PostProcess/SceneFilterRendering.h"
#include "RenderTargets.h"
#include "UnrealMath.h"
#include "Scene.h"
#include "DeferredShading.h"

void RCPassPostProcessAmbientOcclusionSetup::Init()
{
	const D3D_SHADER_MACRO Macros[] = { { "INITIAL_PASS", IsInitialPass() ? "1" : "0" } };
	PSBytecode = CompilePixelShader(TEXT("PostProcessAmbientOcclusion.hlsl"), "MainSetupPS", Macros, sizeof(Macros)/ sizeof(D3D_SHADER_MACRO));
	GetShaderParameterAllocations(PSBytecode, PSParams);
	PS = CreatePixelShader(PSBytecode);

	RenderTargets& SceneContext = RenderTargets::Get();
	GBufferASRV = CreateShaderResourceView2D(SceneContext.GetGBufferATexture(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0);
	GBufferASamplerState = TStaticSamplerState<>::GetRHI();
	GBufferBSRV = CreateShaderResourceView2D(SceneContext.GetGBufferBTexture(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0);
	GBufferBSamplerState = TStaticSamplerState<>::GetRHI();
	SceneDepthSRV = CreateShaderResourceView2D(SceneContext.GetSceneDepthTexture(), DXGI_FORMAT_R24G8_TYPELESS, 1, 0);
	SceneDepthSamplerState = TStaticSamplerState<>::GetRHI();

	RasterState = TStaticRasterizerState<>::GetRHI();
	BlendState = TStaticBlendState<>::GetRHI();

	ID3D11Texture2D* Input = IsInitialPass() ? Input0 : Input1;
	D3D11_TEXTURE2D_DESC InputDesc;
	Input->GetDesc(&InputDesc);
	IntPoint InputExtent(InputDesc.Width,InputDesc.Height);
	OutputExtent = IntPoint::DivideAndRoundUp(InputExtent, 2);
	Output = CreateTexture2D(OutputExtent.X, OutputExtent.Y, DXGI_FORMAT_R32G32B32A32_FLOAT,true, true,false,1);
	OutputRTV = CreateRenderTargetView2D(Output, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
}

void RCPassPostProcessAmbientOcclusionSetup::Process(ViewInfo& View)
{
	D3D11DeviceContext->OMSetRenderTargets(1, &OutputRTV, NULL);

	D3D11DeviceContext->IASetInputLayout(GFilterInputLayout);
	UINT Strides = sizeof(FilterVertex);
	UINT Offset = 0;
	D3D11DeviceContext->IASetVertexBuffers(0,1,&GScreenRectangleVertexBuffer,&Strides,&Offset);
	D3D11DeviceContext->IASetIndexBuffer(GScreenRectangleIndexBuffer,DXGI_FORMAT_R16_UINT,0);

	D3D11DeviceContext->VSSetShader(GCommonPostProcessVS, 0, 0);
	D3D11DeviceContext->PSSetShader(PS, 0, 0);

	const ParameterAllocation& ViewParam = PSParams["View"];
	const ParameterAllocation& GBufferATextureParam = PSParams["SceneTexturesStruct_GBufferATexture"];
	const ParameterAllocation& GBufferATextureSamplerParam = PSParams["SceneTexturesStruct_GBufferATextureSampler"];
// 	const ParameterAllocation& GBufferBTextureParam = PSParams["SceneTexturesStruct_GBufferBTexture"];
// 	const ParameterAllocation& GBufferBTextureSamplerParam = PSParams["SceneTexturesStruct_GBufferBTextureSampler"];
	const ParameterAllocation& SceneDepthTextureParam = PSParams["SceneTexturesStruct_SceneDepthTexture"];
	const ParameterAllocation& SceneDepthTextureSamplerParam = PSParams["SceneTexturesStruct_SceneDepthTextureSampler"];

	D3D11DeviceContext->PSSetConstantBuffers(ViewParam.BufferIndex, 1, &View.ViewUniformBuffer);
	D3D11DeviceContext->PSSetShaderResources(GBufferATextureParam.BaseIndex, GBufferATextureParam.Size, &GBufferASRV);
	D3D11DeviceContext->PSSetSamplers(GBufferATextureSamplerParam.BaseIndex, GBufferATextureSamplerParam.Size, &GBufferASamplerState);
// 	D3D11DeviceContext->PSSetShaderResources(GBufferBTextureParam.BaseIndex, GBufferBTextureParam.Size, &GBufferBSRV);
// 	D3D11DeviceContext->PSSetSamplers(GBufferBTextureSamplerParam.BaseIndex, GBufferBTextureSamplerParam.Size, &GBufferBSamplerState);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &SceneDepthSRV);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &SceneDepthSamplerState);

	if (IsInitialPass())
	{
		const ParameterAllocation& PostprocessInput0SizeParam = PSParams["PostprocessInput0Size"];
		Vector4 PostprocessInput0Size = Vector4((float)OutputExtent.X, (float)OutputExtent.Y, 1.f / OutputExtent.X, 1.f / OutputExtent.Y);
		memcpy(GlobalConstantBufferData + PostprocessInput0SizeParam.BaseIndex, &PostprocessInput0Size, PostprocessInput0SizeParam.Size);
		D3D11DeviceContext->UpdateSubresource(GlobalConstantBuffer, 0, NULL, GlobalConstantBufferData, 0, 0);
		D3D11DeviceContext->PSSetConstantBuffers(PostprocessInput0SizeParam.BufferIndex, 1, &GlobalConstantBuffer);
	}
	else
	{
		const ParameterAllocation& PostprocessInput1SizeParam = PSParams["PostprocessInput1Size"];
		Vector4 PostprocessInput1Size = Vector4(OutputExtent.X, OutputExtent.Y, 1.f / OutputExtent.X, 1.f / OutputExtent.Y);
		memcpy(GlobalConstantBufferData + PostprocessInput1SizeParam.BaseIndex, &PostprocessInput1Size, PostprocessInput1SizeParam.Size);
		D3D11DeviceContext->UpdateSubresource(GlobalConstantBuffer, 0, NULL, GlobalConstantBufferData, 0, 0);
		D3D11DeviceContext->PSSetConstantBuffers(PostprocessInput1SizeParam.BufferIndex, 1, &GlobalConstantBuffer);
	}

	D3D11DeviceContext->RSSetState(RasterState);

	uint32 ScaleFactor = RenderTargets::Get().GetBufferSizeXY().X / OutputExtent.X;
	D3D11_VIEWPORT Viewport;
	Viewport.TopLeftX = GViewport.TopLeftX / ScaleFactor;
	Viewport.TopLeftY = GViewport.TopLeftY / ScaleFactor;
	Viewport.Width = GViewport.Width / ScaleFactor;
	Viewport.Height = GViewport.Height / ScaleFactor;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.0f;
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->OMSetBlendState(BlendState, NULL, 0xffffffff);

	D3D11DeviceContext->DrawIndexed(6, 0, 0);

	ID3D11ShaderResourceView* SRV = NULL;
	ID3D11SamplerState* Sampler = NULL;
	D3D11DeviceContext->PSSetShaderResources(GBufferATextureParam.BaseIndex, GBufferATextureParam.Size, &SRV);
	D3D11DeviceContext->PSSetSamplers(GBufferATextureSamplerParam.BaseIndex, GBufferATextureSamplerParam.Size, &Sampler);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &SRV);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &Sampler);
}

bool RCPassPostProcessAmbientOcclusionSetup::IsInitialPass() const
{
	if (!Input0 && Input1)
	{
		return false;
	}
	if (Input0 && !Input1)
	{
		return true;
	}
	return false;
}

void RCPassPostProcessAmbientOcclusion::Init(bool InAOSetupAsInput)
{
	bAOSetupAsInput = InAOSetupAsInput;

	PSBytecode = CompilePixelShader(TEXT("PostProcessAmbientOcclusion.hlsl"), "MainPS");
	GetShaderParameterAllocations(PSBytecode, PSParams);
	PS = CreatePixelShader(PSBytecode);
	RasterState = TStaticRasterizerState<>::GetRHI();
	BlendState = TStaticBlendState<>::GetRHI();
	if (bAOSetupAsInput)
	{
		D3D11_TEXTURE2D_DESC InputDesc;
		Input0->GetDesc(&InputDesc);
		OutputExtent = IntPoint(InputDesc.Width, InputDesc.Height);
		Output = CreateTexture2D(OutputExtent.X, OutputExtent.Y, DXGI_FORMAT_R8G8B8A8_UNORM, true, true, false, 1);
		OutputRTV = CreateRenderTargetView2D(Output, DXGI_FORMAT_R8G8B8A8_UNORM,0);
	}
	RandomNormalSRV;
	RandomNormalSampler;
	GBufferASRV;
	GBufferASamplerState;
	GBufferBSRV;
	GBufferBSamplerState;
	SceneDepthSRV;
	SceneDepthSamplerState;
	PostprocessInput1;
	PostprocessInput1Sampler;
	PostprocessInput3;
	PostprocessInput3Sampler;
}

void RCPassPostProcessAmbientOcclusion::Process(ViewInfo& View)
{
	ID3D11RenderTargetView* DestRenderTarget = NULL;
	RenderTargets& SceneContext = RenderTargets::Get();

	if (bAOSetupAsInput)
	{
		DestRenderTarget = OutputRTV;
	}
	else
	{

	}

	IntPoint TexSize = Input0 ? OutputExtent : SceneContext.GetBufferSizeXY();

	uint32 ScaleToFullRes = SceneContext.GetBufferSizeXY().X / TexSize.X;

	IntRect ViewRect = IntRect::DivideAndRoundUp(View.ViewRect, ScaleToFullRes);

	const int32 ShaderQuality = 4;

	bool bDoUpsample = (Input2 != 0);

	const ParameterAllocation& ViewParam = PSParams["View"];
	D3D11DeviceContext->PSSetConstantBuffers(ViewParam.BufferIndex, 1, &View.ViewUniformBuffer);
	ProcessPS(DestRenderTarget, ViewRect, TexSize, ShaderQuality, bDoUpsample);
}

void RCPassPostProcessAmbientOcclusion::ProcessPS(ID3D11RenderTargetView* DestRenderTarget, const IntRect& ViewRect, const IntPoint& TexSize, int32 ShaderQuality, bool bDoUpsample)
{
	D3D11DeviceContext->OMSetRenderTargets(1, &DestRenderTarget, NULL);

	D3D11DeviceContext->IASetInputLayout(GFilterInputLayout);
	UINT Strides = sizeof(FilterVertex);
	UINT Offset = 0;
	D3D11DeviceContext->IASetVertexBuffers(0, 1, &GScreenRectangleVertexBuffer, &Strides, &Offset);
	D3D11DeviceContext->IASetIndexBuffer(GScreenRectangleIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	D3D11DeviceContext->VSSetShader(GCommonPostProcessVS, 0, 0);
	D3D11DeviceContext->PSSetShader(PS, 0, 0);

	const ParameterAllocation& HZBRemappingParam = PSParams["HZBRemapping"];
	const ParameterAllocation& PostprocessInput1Param = PSParams["PostprocessInput1"];
	const ParameterAllocation& PostprocessInput1SamplerParam = PSParams["PostprocessInput1Sampler"];
	const ParameterAllocation& PostprocessInput2SizeParam = PSParams["PostprocessInput2Size"];
	const ParameterAllocation& PostprocessInput3Param = PSParams["PostprocessInput3"];
	const ParameterAllocation& PostprocessInput3SamplerParam = PSParams["PostprocessInput3Sampler"];
	const ParameterAllocation& RandomNormalTextureParam = PSParams["RandomNormalTexture"];
	const ParameterAllocation& RandomNormalTextureSamplerParam = PSParams["RandomNormalTextureSampler"];
	const ParameterAllocation& GBufferATextureParam = PSParams["SceneTexturesStruct_GBufferATexture"];
	const ParameterAllocation& GBufferATextureSamplerParam = PSParams["SceneTexturesStruct_GBufferATextureSampler"];
	const ParameterAllocation& GBufferBTextureParam = PSParams["SceneTexturesStruct_GBufferBTexture"];
	const ParameterAllocation& GBufferBTextureSamplerParam = PSParams["SceneTexturesStruct_GBufferBTextureSampler"];
	const ParameterAllocation& SceneDepthTextureParam = PSParams["SceneTexturesStruct_SceneDepthTexture"];
	const ParameterAllocation& SceneDepthTextureSamplerParam = PSParams["SceneTexturesStruct_SceneDepthTextureSampler"];
	const ParameterAllocation& ScreenSpaceAOParamsParam = PSParams["ScreenSpaceAOParams"];


	D3D11DeviceContext->PSSetShaderResources(PostprocessInput1Param.BaseIndex, PostprocessInput1Param.Size, &RandomNormalSRV);
	D3D11DeviceContext->PSSetSamplers(PostprocessInput1SamplerParam.BaseIndex, PostprocessInput1SamplerParam.Size, &RandomNormalSampler);
	D3D11DeviceContext->PSSetShaderResources(PostprocessInput3Param.BaseIndex, PostprocessInput3Param.Size, &RandomNormalSRV);
	D3D11DeviceContext->PSSetSamplers(PostprocessInput3SamplerParam.BaseIndex, PostprocessInput3SamplerParam.Size, &RandomNormalSampler);
	D3D11DeviceContext->PSSetShaderResources(RandomNormalTextureParam.BaseIndex, RandomNormalTextureParam.Size, &RandomNormalSRV);
	D3D11DeviceContext->PSSetSamplers(RandomNormalTextureSamplerParam.BaseIndex, RandomNormalTextureSamplerParam.Size, &RandomNormalSampler);
	D3D11DeviceContext->PSSetShaderResources(GBufferATextureParam.BaseIndex, GBufferATextureParam.Size, &GBufferASRV);
	D3D11DeviceContext->PSSetSamplers(GBufferATextureSamplerParam.BaseIndex, GBufferATextureSamplerParam.Size, &GBufferASamplerState);
	D3D11DeviceContext->PSSetShaderResources(GBufferBTextureParam.BaseIndex, GBufferBTextureParam.Size, &GBufferBSRV);
	D3D11DeviceContext->PSSetSamplers(GBufferBTextureSamplerParam.BaseIndex, GBufferBTextureSamplerParam.Size, &GBufferBSamplerState);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &SceneDepthSRV);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &SceneDepthSamplerState);

	Vector4 HZBRemapping;
	memcpy(GlobalConstantBufferData + HZBRemappingParam.BaseIndex, &HZBRemapping, HZBRemappingParam.Size);
	Vector4 PostprocessInput2Size;
	memcpy(GlobalConstantBufferData + PostprocessInput2SizeParam.BaseIndex, &PostprocessInput2Size, PostprocessInput2SizeParam.Size);
	Vector4 ScreenSpaceAOParams[6];
	memcpy(GlobalConstantBufferData + ScreenSpaceAOParamsParam.BaseIndex, &ScreenSpaceAOParams, ScreenSpaceAOParamsParam.Size);
	D3D11DeviceContext->PSSetConstantBuffers(HZBRemappingParam.BufferIndex, 1, &GlobalConstantBuffer);

	D3D11DeviceContext->RSSetState(RasterState);
	D3D11_VIEWPORT Viewport;
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->OMSetBlendState(BlendState, NULL, 0xffffffff);

	D3D11DeviceContext->DrawIndexed(6, 0, 0);
}

void RCPassPostProcessBasePassAO::Init()
{
	PSBytecode = CompilePixelShader(TEXT("PostProcessAmbientOcclusion.hlsl"), "BasePassAOPS");
	GetShaderParameterAllocations(PSBytecode, PSParams);
	PS = CreatePixelShader(PSBytecode);
	RasterState = TStaticRasterizerState<>::GetRHI();
	BlendState = TStaticBlendState<>::GetRHI();
}

void RCPassPostProcessBasePassAO::Process(ViewInfo& View)
{
	D3D11DeviceContext->OMSetRenderTargets(1, &OutputRTV, NULL);

	D3D11DeviceContext->IASetInputLayout(GFilterInputLayout);
	UINT Strides = sizeof(FilterVertex);
	UINT Offset = 0;
	D3D11DeviceContext->IASetVertexBuffers(0, 1, &GScreenRectangleVertexBuffer, &Strides, &Offset);
	D3D11DeviceContext->IASetIndexBuffer(GScreenRectangleIndexBuffer, DXGI_FORMAT_R16_UINT,0);

	D3D11DeviceContext->VSSetShader(GCommonPostProcessVS, 0, 0);
	D3D11DeviceContext->PSSetShader(PS, 0, 0);

	D3D11DeviceContext->RSSetState(RasterState);
	D3D11_VIEWPORT Viewport;
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->OMSetBlendState(BlendState, NULL, 0xffffffff);

	D3D11DeviceContext->DrawIndexed(6, 0, 0);
}
