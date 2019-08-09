#include "RenderResource.h"

XRenderResource::~XRenderResource()
{
	if (bInitialized)
	{

	}
}

void XRenderResource::InitResource()
{
	if (!bInitialized)
	{
		InitDynamicRHI();
		InitRHI();
		bInitialized = true;
	}
}

void XRenderResource::ReleaseResource()
{
	if (bInitialized)
	{
		ReleaseRHI();
		ReleaseDynamicRHI();
		bInitialized = false;
	}
}

void XRenderResource::UpdateRHI()
{
	if (bInitialized)
	{
		ReleaseRHI();
		ReleaseDynamicRHI();
		InitDynamicRHI();
		InitRHI();
	}
}

