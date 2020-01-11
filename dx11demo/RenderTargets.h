#pragma once

#include "D3D11RHI.h"

struct RenderTarget
{
	ID3D11Texture2D*	Texture2D;
	ID3D11Texture3D*	Texture3D;

	ID3D11ShaderResourceView* SRV;
	ID3D11UnorderedAccessView* UAV;
};

class RenderTargetPool
{

};

class SceneRenderTargets
{
public:

};