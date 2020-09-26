#pragma once

#include "StaticMeshDrawList.h"
#include "Renderer.h"


class Scene
{
public:
	TStaticMeshDrawList<PositionOnlyDepthDrawingPolicy> PositionOnlyDepthDrawList;
	TStaticMeshDrawList<DepthDrawingPolicy> PositionOnlyDepthDrawList;
	TStaticMeshDrawList<DepthDrawingPolicy> MaskedDepthDrawList;
};