#pragma once

class XSocketSubsystem
{
	virtual ~XSocketSubsystem() { }

	virtual class FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, bool bForceUDP = false) = 0;
};