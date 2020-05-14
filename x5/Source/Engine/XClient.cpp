#include "XClient.h"

X5_API const ID3D11Texture2D* FRenderTarget::GetRenderTargetTexture() const
{
	return RenderTargetTextureRHI;
}

