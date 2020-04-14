#pragma once

#include "Math/Vector.h"
#include "ShaderCore/VertexFactory.h"

// Constants.
enum { MAX_TEXCOORDS = 4, MAX_STATIC_TEXCOORDS = 8 };

/** The information used to build a static-mesh vertex. */
struct FStaticMeshBuildVertex
{
	FVector Position;

	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;

	FVector2D UVs[MAX_STATIC_TEXCOORDS];
	FColor Color;
};

struct FStaticMeshDataType
{
	FVertexStreamComponent PositionComponent;
	FVertexStreamComponent TangentBasisComponent[2];
	std::vector<FVertexStreamComponent> TextureCoordinates;
	FVertexStreamComponent ColorComponent;

	ID3D11ShaderResourceView* PositionComponentSRV;
	ID3D11ShaderResourceView* TangentsSRV;
	ID3D11ShaderResourceView* TextureCoordinatesSRV;
	ID3D11ShaderResourceView* ColorComponentSRV;
};