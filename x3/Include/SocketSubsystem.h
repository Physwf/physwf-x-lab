#pragma once

class XInternetAddr;

class XSocketSubsystem
{
	virtual ~XSocketSubsystem() { }

	virtual class XSocket* CreateSocket(bool bForceUDP = false) = 0;

	virtual void DestroySocket(class XSocket* InSocket) = 0;

	virtual bool GetHostByName(const char* HostName, XInternetAddr& OutAddr) = 0;
};