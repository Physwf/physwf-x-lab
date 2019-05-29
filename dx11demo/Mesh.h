#pragma once

#include <vector>

struct SubMesh
{
	struct Vetex
	{
		float x, y, z;
		float u1, v1;
		float u2, v2;
		float nx, ny, nz;
	};

	std::vector<Vetex> mVertices;
	std::vector<unsigned short> mIndices;
};

class Mesh
{
public:
	Mesh() {}
	~Mesh() {}

	void ImportFromFBX(const char* pFileName);
private:
	std::vector<SubMesh> mSubMeshes;
};