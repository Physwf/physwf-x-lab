#pragma once

#include "X5.h"
#include "RenderCore/RenderResource.h"

class FViewport;
class FViewportClient;

class FRenderTarget
{
public:
	virtual ~FRenderTarget() {}

	X5_API virtual const ID3D11Texture2D*& GetRenderTargetTexture() const;
protected:
	ID3D11Texture2D* RenderTargetTextureRHI;
};

class FViewport : public FRenderTarget, protected FRenderResource
{
public:
	X5_API FViewport(FViewportClient* InViewportClient);

	X5_API void Draw(bool bShouldPresent = true);
};