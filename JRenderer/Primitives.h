#pragma once

#include "DirectXMath.h"

#include <vector>

using namespace DirectX;

struct MeshVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Tangent;
	XMFLOAT2 UV;
};

class Mesh
{
public:
	std::vector<MeshVertex> Vertices;
	std::vector<int> Indices;

	void LoadFBX(const char* Filename);
};