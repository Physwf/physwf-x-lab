#pragma once

#include "SocketSubsystem.h"

class XSocketSubsystemBSD : public XSocketSubsystem
{
public:
	virtual class XSocket* CreateSocket(bool bForceUDP /* = false */) override;
	virtual void DestroySocket(class XSocket* InSocket) override;
};