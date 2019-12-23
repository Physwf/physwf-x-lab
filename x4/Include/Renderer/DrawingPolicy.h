#pragma once

#include <d3d11.h>

struct FDrawingPolicyRenderState
{
private:
	ID3D11BlendState* BlendState;
	ID3D11DepthStencilState* DepthStencilState;
};