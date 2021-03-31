#pragma once

#include "DirectXMath.h"

#include <vector>
#include <string>
#include <map>

struct MeshVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Tangent;
	XMFLOAT2 UV;
};

struct ViewUniform
{
	XMFLOAT4 ViewOrigin;
	XMFLOAT4X4 WorldToClip;
	XMFLOAT4X4 ClipToWorld;
	XMFLOAT4X4 SvPositionToWorld;
	XMFLOAT4 ViewSizeAndInvSize;
	XMFLOAT4 ViewRectMin;
	char Pading00[256 - sizeof(XMFLOAT4) * 3 - sizeof(XMFLOAT4X4) * 3];
};

struct PrimitiveUniform
{
	XMFLOAT4X4 LocalToWorld;
	float LocalToWorldDeterminantSign;
	char Pading00[256 - sizeof(XMFLOAT4X4) - sizeof(float)];
};

class Mesh
{
public:
	std::vector<MeshVertex> Vertices;
	std::vector<int> Indices;

	void LoadFBX(const char* Filename);
	void LoadObj(const char* Filename);
};

class ObjLoader
{
	struct VertexIndex
	{
		int PositonIndex;
		int NormalIndex;
		int TexCoordIndex;
	};

	struct Face
	{
		VertexIndex v1;
		VertexIndex v2;
		VertexIndex v3;
		VertexIndex v4;

		int VerticesCount;
	};

	struct SubMesh
	{
		std::string MaterialName;
		std::vector<MeshVertex> Vertices;
		std::vector<unsigned short> Indices;
	};

	struct ObjMaterial
	{
		XMFLOAT3 Ka;
		XMFLOAT3 Kd;
		XMFLOAT3 Ks;
		XMFLOAT3 Ke;
		XMFLOAT3 Tf;
		float Ns;
		float Ni;
		float d;
		float illum;
	};

public:
	bool Load(const char* filename);
private:
	bool ParseMaterial(const char* filename);
	bool ParseLine(const std::string& line);
	bool ParseName(const std::string& line, std::string& Name);
	bool ParsePosition(const std::string& line, XMFLOAT3& Position);
	bool ParseNormal(const std::string& line, XMFLOAT3& Normal);
	bool ParseTextCoord2D(const std::string& line, XMFLOAT2& TexCoord);
	bool ParseColor(const std::string& line, XMFLOAT3& Color);
	bool ParseScalar(const std::string& line, float& Value);
	bool ParseFace(const std::string& line, Face& F);
	bool ParseVertexIndex(const std::string& line, VertexIndex& Index);
	void AssembleSubMesh(SubMesh* CurrentSubMesh, const Face& F);
private:
	std::vector<XMFLOAT3> Positions;
	std::vector<XMFLOAT3> Normals;
	std::vector<XMFLOAT2> TexCoords;

	std::map<std::string, SubMesh> SubMeshes;
	std::map<std::string, ObjMaterial> Materials;
	SubMesh* CurrentSubMesh;
	ObjMaterial* CurrentMaterial;
	std::string ResourceDir;
};