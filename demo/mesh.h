#pragma once

#include <string>
#include <map>
#include <vector>


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}

	struct Vector3
	{
		float x, y, z;
	};

	struct Vector2
	{
		float x, y;
	};

	struct Vertex
	{
		Vector3		Position;
		Vector3		Normal;
		Vector2		Tex;
	};

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
	};

	struct SubMesh
	{
		std::vector<Vertex> Vertices;
		std::vector<unsigned short> Indices;
	};

	bool LoadFromObj(const char* filename);

	bool ParseLine(const class std::string& line);
	bool ParseName(const std::string& line, std::string& Name);
	bool ParsePosition(const std::string& line, Vector3& Position);
	bool ParseTextCoord2D(const std::string& line, Vector2& TexCoord);
	bool ParseFace(const std::string& line, Face& F);
	bool ParseVertexIndex(const std::string& line, VertexIndex& Index);
	void AssembleSubMesh(SubMesh* CurrentSubMesh,const Face& F);

	const std::map<std::string, SubMesh>& GetSubMeshes() const
	{
		return SubMeshes;
	}
private:
	std::vector<Vector3> Positions;
	std::vector<Vector2> TexCoords;

	std::map<std::string, SubMesh> SubMeshes;
	SubMesh* CurrentSubMesh;
};