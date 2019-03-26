#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Sockets.h"

#include <iostream>

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
			while (true)
			{
				std::cout << "server accept start" << std::endl;
				XSocket* NewSocket = Socket->Accept();
				std::cout << "server accept end" << std::endl;
			}
			std::cout << "server accept failed" << std::endl;
		}
	}
}

void socket_client()
{
	XSocket* Socket = ISocketSubsystem::Get()->CreateSocket(1);
	XInternetAddr* InternetAddr = ISocketSubsystem::Get()->CreateInternetAddr();
	bool bValid = false;
	InternetAddr->SetIp("127.0.0.1", bValid);
	InternetAddr->SetPort(9999);
	bool bResult = Socket->Connect(*InternetAddr);
	if (bResult)
	{
		int i = 0;
		while (true)
		{
			int BytesSent = 0;
			bool bSendResult = Socket->Send((unsigned char*)&i, sizeof(int), BytesSent);
			if (bSendResult)
			{
				std::cout << "client send " << BytesSent << "bytes, Send content: " << i++ << std::endl;
			}
			else
			{
				std::cout << "client send failed! Send content: " << i << std::endl;
			}
		}
	}
}