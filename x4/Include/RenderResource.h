#pragma once

#include "RHI.h"

class XRenderResource
{
public:

	XRenderResource()
		:FeatureLevel(ERHIFeatureLevel::Num),
		,bInitialized(false)
	{}

	XRenderResource(ERHIFeatureLevel::Type InFeatureLevel)
		:FeatureLevel(InFeatureLevel),
		, bInitialized(false)
	{}

	virtual ~XRenderResource();

	virtual void InitDynamicRHI() {}

	virtual void ReleaseDynamicRHI() {}

	virtual void InitRHI() {}

	virtual void ReleaseRHI() {}

	virtual void InitResource();

	virtual void ReleaseResource();

	void UpdateRHI();

	bool IsInitialized() const { return bInitialized; }

protected:
	ERHIFeatureLevel::Type GetFeatureLevel();
	bool HasValidFeatureLevel() const { return FeatureLevel < ERHIFeatureLevel::Num; }
private:
	ERHIFeatureLevel::Type FeatureLevel;
	bool bInitialized;
};

class XVertexBuffer : public XRenderResource
{
public:
	XVertexBufferRef VertexBufferRHI;

	virtual ~XVertexBuffer() {}

	virtual void ReleaseRHI() override
	{
		VertexBufferRHI.SafeRelease();
	}
};

class XIndexBuffer : public XRenderResource
{
public:
	XIndexBufferRef IndexBufferRHI;

	virtual ~XIndexBuffer() {}

	virtual void ReleaseRHI() override
	{
		IndexBufferRHI.SafeRelease();
	}
};