#include "DrawingPolicy.h"
#include "log.h"

void DrawingPolicy::Draw(const ShaderPipeline* Pipeline)
{

}

void DepthOnlyDrawingPolicy::InitResource()
{
	HRESULT hr;

	mViewport.Width = (FLOAT)WindowWidth;
	mViewport.Height = (FLOAT)WindowHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	hr = D3D11Device->CreateRasterizerState(&wfdesc, &mRasterState);


	mBlendState = nullptr;
}

void DepthOnlyDrawingPolicy::ReleaseResource()
{
	if (mRasterState)
	{
		mRasterState->Release();
	}
}

void DepthOnlyDrawingPolicy::SetRenderState()
{
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->RSSetState(mRasterState);
}

void DepthOnlyDrawingPolicy::Draw(const ShaderPipeline* Pipeline)
{
	D3D11DeviceContext->VSSetShader(Pipeline->VertexShader);
	D3D11DeviceContext->PSSetShader(Pipeline->PixelShader);
}

void ShadowDepthDrawingPolicy::InitResource()
{
	HRESULT hr;

	mViewport.Width = (FLOAT)WindowWidth;
	mViewport.Height = (FLOAT)WindowHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	hr = D3D11Device->CreateRasterizerState(&wfdesc, &mRasterState);

	mBlendState = nullptr;
}

void ShadowDepthDrawingPolicy::ReleaseResource()
{
	if (mRasterState)
	{
		mRasterState->Release();
	}
}

void ShadowDepthDrawingPolicy::SetRenderState()
{
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->RSSetState(RasterState);
}

void ShadowDepthDrawingPolicy::Draw(const ShaderPipeline* Pipeline)
{
	D3D11DeviceContext->VSSetShader(Pipeline->VertexShader);
	D3D11DeviceContext->PSSetShader(Pipeline->PixelShader);
}

void BaseDrawingPolicy::InitResource()
{
	HRESULT hr;

	mViewport.Width = (FLOAT)WindowWidth;
	mViewport.Height = (FLOAT)WindowHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	hr = D3D11Device->CreateRasterizerState(&wfdesc, &mRasterState);

	mBlendState = nullptr;
}

void BaseDrawingPolicy::ReleaseResource()
{
	if (mRasterState)
	{
		mRasterState->Release();
	}
}

void BaseDrawingPolicy::SetRenderState()
{
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->RSSetState(RasterState);
}

void BaseDrawingPolicy::Draw(const ShaderPipeline* Pipeline)
{
	D3D11DeviceContext->VSSetShader(Pipeline->VertexShader);
	D3D11DeviceContext->PSSetShader(Pipeline->PixelShader);
}
