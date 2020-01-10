#pragma once

#include "D3D11RHI.h"

class RenderPass
{
public:
	virtual void InitResource() = 0;
	virtual void ReleaseResource() = 0;
	virtual void SetRenderState() = 0;
	virtual void Draw() = 0;

};

class DepthOnlyRenderPass : public RenderPass
{
public:
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void SetRenderState() override;
	virtual void Draw() override;
private:
	ID3D11DepthStencilView* mDepthStencialView = NULL;
	ID3D11Texture2D* mDepthStencialBuffer = NULL;
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mRasterState = NULL;
};

class ShadowMapPass : public RenderPass
{
public:
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void SetRenderState() override;
	virtual void Draw() override;
private:
	ID3D11DepthStencilView* mDepthStencialView = NULL;
	ID3D11Texture2D* mDepthStencialBuffer = NULL;
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mRasterState = NULL;
};

class BaseRenderPass : public RenderPass
{
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void SetRenderState() override;
	virtual void Draw() override;
private:
	ID3D11RenderTargetView* mRenderTargetView = NULL;
	ID3D11DepthStencilView* mDepthStencialView = NULL;
	ID3D11Texture2D* mColorBuffer = NULL;
	ID3D11Texture2D* mDepthStencialBuffer = NULL;
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mRasterState = NULL;
};