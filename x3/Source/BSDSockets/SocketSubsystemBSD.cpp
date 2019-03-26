#include "SocketSubsystemBSD.h"
#include "SocketBSD.h"

class XSocket* XSocketSubsystemBSD::CreateSocket(int SocketType, bool bForceUDP /* = false */)
{
	SOCKET Socket = INVALID_SOCKET;
	XSocket* Result = nullptr;
	int PlatformSpecificTypeFlags = 0;

#if PLATFORM_HAS_BSD_SOCKET_FEATURE_CLOSE_ON_EXEC
	PlatformSpecificTypeFlags = SOCK_CLOEXEC;
#endif // PLATFORM_HAS_BSD_SOCKET_FEATURE_CLOSE_ON_EXEC

	switch (SocketType)
	{
	case 0:
		Socket = socket(AF_INET, SOCK_DGRAM | PlatformSpecificTypeFlags, IPPROTO_UDP);
		Result = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, SOCKTYPE_Datagram) : nullptr;
		break;
	case 1:
		Socket = socket(AF_INET, SOCK_STREAM | PlatformSpecificTypeFlags, IPPROTO_TCP);
		Result = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, SOCKTYPE_Sreaming) : nullptr;
		break;
	}
	return Result;
}

void XSocketSubsystemBSD::DestroySocket(class XSocket* InSocket)
{

}

class XInternetAddr* XSocketSubsystemBSD::CreateInternetAddr(unsigned int Address, int Port)
{
	XInternetAddrBSD* InternetAddr = new XInternetAddrBSD();
	InternetAddr->SetIp(Address);
	InternetAddr->SetPort(Port);
	return InternetAddr;
}

bool XSocketSubsystemBSD::GetHostByName(const char* HostName, XInternetAddr& OutAddr)
{
	return false;
}

class XSocketBSD* XSocketSubsystemBSD::InternalBSDSocketFactory(SOCKET Socket, ESocketType eSocketType)
{
	return new XSocketBSD(Socket, eSocketType, this);
}
