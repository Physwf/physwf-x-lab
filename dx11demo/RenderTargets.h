#pragma once

class RenderTargets
{
public:

private:
	ID3D11DepthStencilView * ShadowPassDSV;


	
	ID3D11Texture2D* ShadowProjectionRT;
	ID3D11RenderTargetView* ShadowProjectionRTV;


	ID3D11Texture2D* BasePassSceneColorRT;
	ID3D11RenderTargetView* BasePassSceneColorRTV;
	ID3D11Texture2D* BasePassGBufferRT[6];
	ID3D11RenderTargetView* BasePassGBufferRTV[6];

	//G-Buffer
	ID3D11ShaderResourceView* LightPassGBufferSRV[6];
	ID3D11SamplerState* LightPassGBufferSamplerState[6];
	//Screen Space AO, Custom Depth, Custom Stencil
	ID3D11Texture2D* LightPassScreenSapceAOTexture;
	ID3D11Texture2D* LightPassCustomDepthTexture;
	ID3D11Texture2D* LightPassCustomStencialTexture;
};