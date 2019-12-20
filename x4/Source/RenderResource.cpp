#include "RenderResource.h"

std::list<FRenderResource*>*& FRenderResource::GetResourceList()
{
	static std::list<FRenderResource*>* FirstResourceLink = NULL;
	return FirstResourceLink;
}

FRenderResource::~FRenderResource()
{

}

void FRenderResource::InitResource()
{
	if (!bInitialized)
	{
		InitDynamicRHI();
		InitRHI();
		bInitialized = true;
	}
}

void FRenderResource::ReleaseResource()
{
	if (bInitialized)
	{
		ReleaseRHI();
		ReleaseDynamicRHI();
		bInitialized = false;
	}
}

void FRenderResource::UpdateRHI()
{
	if (bInitialized)
	{
		ReleaseRHI();
		ReleaseDynamicRHI();
		InitDynamicRHI();
		InitRHI();
	}
}

void FRenderResource::InitResourceFromPossiblyParallelRendering()
{

}

