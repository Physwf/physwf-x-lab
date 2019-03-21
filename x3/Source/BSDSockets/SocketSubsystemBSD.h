#pragma once

class XSocketSubsystemBSD : public XSocketSubsystem
{
	virtual class XSocket* CreateSocket(bool bForceUDP /* = false */) override;
	virtual void DestroySocket(class XSocket* InSocket) override;
};