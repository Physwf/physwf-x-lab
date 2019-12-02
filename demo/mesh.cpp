#include "Mesh.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <cmath>

bool Mesh::LoadFromObj(const char* filename)
{
	std::string line;
	std::ifstream fs(filename);
	if (fs)
	{
		while (std::getline(fs, line))
		{
			if(line.length() == 0) continue;
			if (!ParseLine(line)) return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool string_startwith(const std::string& str, const char* start)
{
	auto start_len = strlen(start);
	if (str.length() < start_len) return false;
	for (std::string::size_type i = 0; i < start_len; ++i)
	{
		if(str[i] == start[i]) continue;
		return false;
	}
	return true;
}

bool string_split(const std::string& str,const std::string& dilimer, std::vector<std::string>& split_result)
{
	std::string::size_type pos1, pos2;
	pos2 = str.find(dilimer);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		split_result.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + dilimer.size();
		pos2 = str.find(dilimer, pos1);
	}
	if (pos1 != str.length())
		split_result.push_back(str.substr(pos1));
	return split_result.size() > 0;
}

bool Mesh::ParseLine(const class std::string& line)
{
	if (line.length() <= 0) return false;
	if (string_startwith(line, "#"))
	{
	}
	else if (string_startwith(line,"g"))
	{
		std::string GroupName;
		if(!ParseName(line, GroupName)) return false;
		SubMeshes[GroupName] = SubMesh();
		CurrentSubMesh = &SubMeshes[GroupName];
	}
	else if (string_startwith(line, "usemtl"))
	{

	}
	else if (string_startwith(line, "vt"))
	{
		Vector2 TexCoord;
		if (!ParseTextCoord2D(line, TexCoord)) return false;
		TexCoords.push_back(TexCoord);
	}
	else if (string_startwith(line, "v"))
	{
		Vector3 Position;
		if (!ParsePosition(line, Position)) return false;
		Positions.push_back(Position);
	}
	else if (string_startwith(line, "f"))
	{
		Face F;
		if (!ParseFace(line, F)) return false;
		AssembleSubMesh(CurrentSubMesh, F);
	}
	return true;
}

bool Mesh::ParseName(const std::string& line,std::string& Name)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 1)
		{
			Name = result[1];
			return true;
		}
	}
	return false;
}

bool Mesh::ParsePosition(const std::string& line, Vector3& Position)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Position.x = (float)std::atof(result[1].c_str());
			Position.y = (float)std::atof(result[2].c_str());
			Position.z = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}

bool Mesh::ParseTextCoord2D(const std::string& line,Vector2& TexCoord)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 2)
		{
			TexCoord.x = (float)std::atof(result[1].c_str());
			TexCoord.y = (float)std::atof(result[2].c_str());
			return true;
		}
	}
	return false;
}

bool Mesh::ParseFace(const std::string& line, Face& F)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			VertexIndex Index;
			if (!ParseVertexIndex(result[1], Index)) return false;
			F.v1 = Index;
			if (!ParseVertexIndex(result[2], Index)) return false;
			F.v2 = Index;
			if (!ParseVertexIndex(result[3], Index)) return false;
			F.v3 = Index;
			return true;
		}
	}
	return false;
}

bool Mesh::ParseVertexIndex(const std::string& line, VertexIndex& Index)
{
	std::vector<std::string> result;
	if (string_split(line, "/", result))
	{
		if (result.size() >= 2)
		{
			Index.PositonIndex = std::atoi(result[0].c_str());
			Index.TexCoordIndex = std::atoi(result[1].c_str());
			return true;
		}
		else if (result.size() >= 3)
		{
			Index.PositonIndex = std::atoi(result[0].c_str());
			Index.NormalIndex = std::atoi(result[1].c_str());
			Index.TexCoordIndex = std::atoi(result[2].c_str());
			return true;
		}
	}
	return false;
}

void Mesh::AssembleSubMesh(SubMesh* pSubMesh, const Face& F)
{
	if (pSubMesh)
	{
		Vertex V;
		V.Position = Positions[F.v1.PositonIndex-1];
		//V.Normal = Positions[F.v1.NormalIndex];
		V.Tex = TexCoords[F.v1.TexCoordIndex - 1];
		pSubMesh->Vertices.push_back(V);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);

		V.Position = Positions[F.v2.PositonIndex - 1];
		//V.Normal = Positions[F.v2.NormalIndex];
		V.Tex = TexCoords[F.v2.TexCoordIndex - 1];
		pSubMesh->Vertices.push_back(V);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);

		V.Position = Positions[F.v3.PositonIndex - 1];
		//V.Normal = Positions[F.v3.NormalIndex];
		V.Tex = TexCoords[F.v3.TexCoordIndex - 1];
		pSubMesh->Vertices.push_back(V);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
	}
}

