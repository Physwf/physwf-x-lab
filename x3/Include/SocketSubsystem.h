#pragma once

#include "SocketTypes.h"

class XInternetAddr;

class ISocketSubsystem
{
public:
	static ISocketSubsystem* Get();

	virtual ~ISocketSubsystem() { }

	virtual bool Init() = 0;

	virtual void Shutdown() = 0;

	virtual class XSocket* CreateSocket(int SocketType, bool bForceUDP = false) = 0;

	virtual class XInternetAddr* CreateInternetAddr(unsigned int Address = 0, int Port = 0) = 0;

	virtual void DestroySocket(class XSocket* InSocket) = 0;

	virtual bool GetHostByName(const char* HostName, XInternetAddr& OutAddr) = 0;

	virtual ESocketErrors GetLastErrorCode() = 0;

	virtual ESocketErrors TranslateErrorCode(int Code) = 0;
};

