#pragma once

#include "D3D11RHI.h"

void InitInput();
void UpdateView();
void RenderPrePass();
void RenderShadowPass();
void RenderBasePass();
void RenderLight();
