#pragma once

#include "X4.h"
#include <list>
#include <string>
#include <d3d11.h>

class X4_API FRenderResource
{
public:

	/** @return The global initialized resource list. */
	static std::list<FRenderResource*>*& GetResourceList();

	/** Default constructor. */
	FRenderResource():
		 bInitialized(false)
	{}

	/** Destructor used to catch unreleased resources. */
	virtual ~FRenderResource();

	/**
	 * Initializes the dynamic RHI resource and/or RHI render target used by this resource.
	 * Called when the resource is initialized, or when reseting all RHI resources.
	 * Resources that need to initialize after a D3D device reset must implement this function.
	 * This is only called by the rendering thread.
	 */
	virtual void InitDynamicRHI() {}

	/**
	 * Releases the dynamic RHI resource and/or RHI render target resources used by this resource.
	 * Called when the resource is released, or when reseting all RHI resources.
	 * Resources that need to release before a D3D device reset must implement this function.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseDynamicRHI() {}

	/**
	 * Initializes the RHI resources used by this resource.
	 * Called when entering the state where both the resource and the RHI have been initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() {}

	/**
	 * Releases the RHI resources used by this resource.
	 * Called when leaving the state where both the resource and the RHI have been initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() {}

	/**
	 * Initializes the resource.
	 * This is only called by the rendering thread.
	 */
	virtual void InitResource();

	/**
	 * Prepares the resource for deletion.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseResource();

	/**
	 * If the resource's RHI resources have been initialized, then release and reinitialize it.  Otherwise, do nothing.
	 * This is only called by the rendering thread.
	 */
	void UpdateRHI();

	// Probably temporary code that sends a task back to renderthread_local and blocks waiting for it to call InitResource
	void InitResourceFromPossiblyParallelRendering();

	/** @return The resource's friendly name.  Typically a UObject name. */
	virtual std::string GetFriendlyName() const { return "undefined"; }

	// Accessors.
	inline bool IsInitialized() const { return bInitialized; }


private:
	/** This resource's link in the global resource list. */
	//std::list<FRenderResource*> ResourceLink;

	/** True if the resource has been initialized. */
	bool bInitialized;
};

extern X4_API void BeginInitResource(FRenderResource* Resource);
extern X4_API void BeginUpdateResourceRHI(FRenderResource* Resource);
extern X4_API void BeginReleaseResource(FRenderResource* Resource);
extern X4_API void StartBatchedRelease(FRenderResource* Resource);
extern X4_API void EndBatchedRelease(FRenderResource* Resource);
extern X4_API void ReleaseResourceAndFlush(FRenderResource* Resource);

#define SafeRelase(Resource)			if(Resource) { Resource->Release(); Resource = nullptr; }

template<class ResourceType>
class TGlobalResource : public ResourceType
{
public:
	TGlobalResource()
	{
		InitGlobalResource();
	}

	template<typename T1>
	explicit TGlobalResource(T1 Param1)
		:ResourceType(Param1)
	{
		InitGlobalResource();
	}

	/** Initialization constructor: 2 parameters. */
	template<typename T1, typename T2>
	explicit TGlobalResource(T1 Param1, T2 Param2)
		: ResourceType(Param1, Param2)
	{
		InitGlobalResource();
	}

	/** Initialization constructor: 3 parameters. */
	template<typename T1, typename T2, typename T3>
	explicit TGlobalResource(T1 Param1, T2 Param2, T3 Param3)
		: ResourceType(Param1, Param2, Param3)
	{
		InitGlobalResource();
	}

	/** Destructor. */
	virtual ~TGlobalResource()
	{
		ReleaseGlobalResource();
	}

private:
	void InitGlobalResource()
	{
		BeginInitResource((ResourceType*)this);
	}

	void ReleaseGlobalResource()
	{
		((ResourceType*)this)->ReleaseResource();
	}
};
/** A textures resource. */
class FTexture : public FRenderResource
{
public:

};

class X4_API FTextureReference : public FRenderResource
{
public:

};

class X4_API FVertexBuffer : public FRenderResource
{
public:
	ID3D11Buffer* VertexBufferRHI;

	/** Destructor. */
	virtual ~FVertexBuffer() {}

	// FRenderResource interface.
	virtual void ReleaseRHI() override
	{
		SafeRelase(VertexBufferRHI);
	}
	virtual std::string GetFriendlyName() const override { return ("FVertexBuffer"); }
};

class FNullColorVertexBuffer : public FVertexBuffer
{
public:

	virtual void InitRHI() override
	{
		//FRHICreate
	}

	virtual void ReleaseRHI() override
	{
		SafeRelase(VertexBufferSRV);
		FVertexBuffer::ReleaseRHI();
	}

	ID3D11ShaderResourceView* VertexBufferSRV;
};

extern X4_API TGlobalResource<FNullColorVertexBuffer> GNullColorVertexBuffer;

/** An index buffer resource. */
class FIndexBuffer : public FRenderResource
{
public:
	ID3D11Buffer* IndexBufferRHI;

	/** Destructor. */
	virtual ~FIndexBuffer() {}

	// FRenderResource interface.
	virtual void ReleaseRHI() override
	{
		SafeRelase(IndexBufferRHI);
	}
	virtual std::string GetFriendlyName() const override { return ("FIndexBuffer"); }
};
