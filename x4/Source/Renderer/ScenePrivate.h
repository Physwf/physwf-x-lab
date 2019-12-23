#pragma once

#include "SceneInterface.h"


class FScene : public FSceneInterface
{
	virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
	virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;
};

