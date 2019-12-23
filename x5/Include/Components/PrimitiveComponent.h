#pragma once

#include "ActorComponent.h"

class UPrimitiveComponent : public UActorComponent
{
protected:
	virtual void CreateRenderState_Concurrent() override;
};