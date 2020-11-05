#pragma once

#include "D3D11RHI.h"

extern ID3D11Buffer* GlobalConstantBuffer;
extern char GlobalConstantBufferData[4096];

void InitShading();
void Render();