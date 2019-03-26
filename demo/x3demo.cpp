#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Sockets.h"

void socket_server()
{
	XSocket* Socket = ISocketSubsystem::Get()->CreateSocket(1);
	XInternetAddr* InternetAddr = ISocketSubsystem::Get()->CreateInternetAddr();
	InternetAddr->SetAnyAddress();
	InternetAddr->SetPort(9999);
	bool bResult = Socket->Bind(*InternetAddr);
	if (bResult)
	{
		bResult = Socket->Listen(10);
		if (bResult)
		{
			XSocket* NewSocket = Socket->Accept();
		}
	}
}

void socket_client()
{
	XSocket* Socket = ISocketSubsystem::Get()->CreateSocket(1);
	XInternetAddr* InternetAddr = ISocketSubsystem::Get()->CreateInternetAddr();
	bool bValid = false;
	InternetAddr->SetIp("192.168.1.38", bValid);
	InternetAddr->SetPort(9999);
	bool bResult = Socket->Connect(*InternetAddr);
}