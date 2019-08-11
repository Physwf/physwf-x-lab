#pragma once

#include "CoreTypes.h"

class XResourceArrayInterface
{
public:
	virtual const void* GetResourceData() const = 0;

	virtual uint32 GetResourceDataSize() const = 0;

	virtual void Descard() = 0;

	virtual bool IsStatic() = 0;

	virtual bool GetAllowCPUAccess() const = 0;

	virtual void SetAllowCPUAccess(bool bInNeedsCPUAccess) = 0;
};