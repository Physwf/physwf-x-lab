#pragma once

class XInternetAddr;

class XSocketSubsystem
{
public:
	static XSocketSubsystem* Get();

	virtual ~XSocketSubsystem() { }

	virtual bool Init() = 0;

	virtual void Shutdown() = 0;

	virtual class XSocket* CreateSocket(bool bForceUDP = false) = 0;

	virtual void DestroySocket(class XSocket* InSocket) = 0;

	virtual bool GetHostByName(const char* HostName, XInternetAddr& OutAddr) = 0;
};

