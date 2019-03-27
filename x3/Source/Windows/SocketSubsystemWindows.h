#pragma once

#include "BSDSockets/SocketSubsystemBSD.h"

class XSocketSubsystemWindows : public XSocketSubsystemBSD
{
public:
	static XSocketSubsystemWindows* Create();

	static void Destroy();

	virtual bool Init() override;

	virtual void Shutdown() override;

	virtual class XSocket* CreateSocket(int SocketType, bool bForceUDP) override;

	virtual ESocketErrors GetLastErrorCode() override;

	virtual ESocketErrors TranslateErrorCode(int Code) override;
protected:
	static XSocketSubsystemWindows* SocketSingleton;
};