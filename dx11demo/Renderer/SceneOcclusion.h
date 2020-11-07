#pragma once

#include "D3D11RHI.h"
#include "UnrealMath.h"

#define MAXNumMips 10
extern ID3D11ShaderResourceView* HZBSRVs[MAXNumMips];

void InitHZB();
void BuildHZB();
