#pragma once

#include <string>

namespace MeshAttribute
{
	namespace Vertex
	{
		extern  const std::string Position;
		extern  const std::string CornerSharpness;
	}

	namespace VertexInstance
	{
		extern  const std::string TextureCoordinate;
		extern  const std::string Normal;
		extern  const std::string Tangent;
		extern  const std::string BinormalSign;
		extern  const std::string Color;
	}

	namespace Edge
	{
		extern  const std::string IsHard;
		extern  const std::string IsUVSeam;
		extern  const std::string CreaseSharpness;
	}

	namespace Polygon
	{
		extern  const std::string Normal;
		extern  const std::string Tangent;
		extern  const std::string Binormal;
		extern  const std::string Center;
	}

	namespace PolygonGroup
	{
		extern  const std::string ImportedMaterialSlotName;
		extern  const std::string EnableCollision;
		extern  const std::string CastShadow;
	}
}

