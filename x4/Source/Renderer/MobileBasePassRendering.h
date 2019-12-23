#pragma once

#include "ScenePrivate.h"

class FMobileBasePassOpaqueDrawingPolicyFactory
{
public:
	enum { bAllowSimpleElements = true; };
	struct ContexType
	{
		ContexType()
		{}
	};

	static void AddStaticMesh(FScene* Scene, FStaticMesh* StaticMesh);
	static bool DrawDyanamicMesh();
};