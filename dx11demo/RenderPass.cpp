#include "RenderPass.h"
#include "log.h"

void DepthOnlyRenderPass::InitResource()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC DepthStencialDesc;
	ZeroMemory(&DepthStencialDesc, sizeof(D3D11_TEXTURE2D_DESC));
	DepthStencialDesc.Width = WindowWidth;
	DepthStencialDesc.Height = WindowHeight;
	DepthStencialDesc.MipLevels = 1;
	DepthStencialDesc.ArraySize = 1;
	DepthStencialDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencialDesc.SampleDesc.Count = 8;
	DepthStencialDesc.SampleDesc.Quality = 0;
	DepthStencialDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencialDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = D3D11Device->CreateTexture2D(&DepthStencialDesc, 0, &mDepthStencialBuffer);
	if (FAILED(hr))
	{
		X_LOG("CreateTexture2D failed!");
		return;
	}
	hr = D3D11Device->CreateDepthStencilView(mDepthStencialBuffer, 0, &mDepthStencialView);
	if (FAILED(hr))
	{
		X_LOG("CreateDepthStencilView failed!");
		return;
	}


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
}

void DepthOnlyRenderPass::ReleaseResource()
{

}

void DepthOnlyRenderPass::SetRenderState()
{
	D3D11DeviceContext->OMSetRenderTargets(1, NULL, mDepthStencialView);
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->RSSetState(mRasterState);
}

void DepthOnlyRenderPass::Draw()
{

}

void BaseRenderPass::InitResource()
{
	HRESULT hr;
	hr = DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mColorBuffer);
	if (FAILED(hr))
	{
		X_LOG("GetBuffer failed!");
		return;
	}
	hr = D3D11Device->CreateRenderTargetView(mColorBuffer, NULL, &RenderTargetView);
	if (FAILED(hr))
	{
		X_LOG("CreateRenderTargetView failed!");
		return;
	}
	//mBackBuffer->Release();


	D3D11_TEXTURE2D_DESC DepthStencialDesc;
	ZeroMemory(&DepthStencialDesc, sizeof(D3D11_TEXTURE2D_DESC));
	DepthStencialDesc.Width = WindowWidth;
	DepthStencialDesc.Height = WindowHeight;
	DepthStencialDesc.MipLevels = 1;
	DepthStencialDesc.ArraySize = 1;
	DepthStencialDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencialDesc.SampleDesc.Count = 8;
	DepthStencialDesc.SampleDesc.Quality = 0;
	DepthStencialDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencialDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = D3D11Device->CreateTexture2D(&DepthStencialDesc, 0, &mDepthStencialBuffer);
	if (FAILED(hr))
	{
		X_LOG("CreateTexture2D failed!");
		return;
	}
	hr = D3D11Device->CreateDepthStencilView(mDepthStencialBuffer, 0, &mDepthStencialView);
	if (FAILED(hr))
	{
		X_LOG("CreateDepthStencilView failed!");
		return;
	}


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
}

void BaseRenderPass::ReleaseResource()
{
	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
	}
	if (mDepthStencialView)
	{
		mDepthStencialView->Release();
	}
	if (mColorBuffer)
	{
		mColorBuffer->Release();
	}
	if (mDepthStencialBuffer)
	{
		mDepthStencialBuffer->Release();
	}
	if (mRasterState)
	{
		mRasterState->Release();
	}
}

void BaseRenderPass::SetRenderState()
{
	D3D11DeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencialView);
	D3D11DeviceContext->RSSetViewports(1, &Viewport);
	D3D11DeviceContext->RSSetState(RasterState);
}

void BaseRenderPass::Draw()
{

}
