#pragma once

#include "D3D11RHI.h"
#include "Shader.h"

class DrawingPolicy
{
public:
	virtual void InitResource() = 0;
	virtual void ReleaseResource() = 0;
	virtual void SetRenderState() = 0;
	virtual void Draw(const ShaderPipeline* Pipeline);
};

class DepthOnlyDrawingPolicy : public DrawingPolicy
{
public:
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void SetRenderState() override;
	virtual void Draw(const ShaderPipeline* Pipeline) override;
private:
	ID3D11BlendState* mBlendState;
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mRasterState = NULL;
};

class ShadowDepthDrawingPolicy : public DrawingPolicy
{
public:
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void SetRenderState() override;
	virtual void Draw(const ShaderPipeline* Pipeline) override;
private:
	ID3D11BlendState* mBlendState;
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mRasterState = NULL;
};

class BaseDrawingPolicy : public DrawingPolicy
{
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void SetRenderState() override;
	virtual void Draw(const ShaderPipeline* Pipeline) override;
private:
	ID3D11BlendState* mBlendState;
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mRasterState = NULL;
};