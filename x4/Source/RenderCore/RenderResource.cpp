#include "RenderCore/RenderResource.h"

TLinkedList<FRenderResource*>*& FRenderResource::GetResourceList()
{
	static TLinkedList<FRenderResource*>* FirstResourceLink = NULL;
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

extern X4_API void BeginInitResource(FRenderResource* Resource)
{
	Resource->InitResource();
}

extern X4_API void BeginUpdateResourceRHI(FRenderResource* Resource)
{
	Resource->UpdateRHI();
}

extern X4_API void BeginReleaseResource(FRenderResource* Resource)
{

}

extern X4_API void StartBatchedRelease(FRenderResource* Resource)
{

}

extern X4_API void EndBatchedRelease(FRenderResource* Resource)
{

}

extern X4_API void ReleaseResourceAndFlush(FRenderResource* Resource)
{

}

TGlobalResource<FNullColorVertexBuffer> GNullColorVertexBuffer;