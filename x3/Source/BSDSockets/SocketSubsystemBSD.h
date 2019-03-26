#pragma once

#include "SocketSubsystem.h"
#include "SocketSubsystemBSDPrivate.h"

class XSocketSubsystemBSD : public ISocketSubsystem
{
public:
	virtual class XSocket* CreateSocket(int SocketType, bool bForceUDP /* = false */) override;

	virtual void DestroySocket(class XSocket* InSocket) override;

	virtual class XInternetAddr* CreateInternetAddr(unsigned int Address, int Port) override;

	virtual bool GetHostByName(const char* HostName, XInternetAddr& OutAddr) override;

protected:
	virtual class XSocketBSD* InternalBSDSocketFactory(SOCKET Socket, ESocketType eSocketType);

	friend XSocketBSD;
};