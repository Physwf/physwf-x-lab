#pragma once

#include "BSDSockets/SocketSubsystemBSD.h"

class XSocketSubsystemWindows : public XSocketSubsystemBSD
{

	virtual bool Init() override;

	virtual void Shutdown() override;
};