#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Sockets.h"

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

std::vector<XSocket*> AllClient;
std::mutex LogMutex;
#define LOG_GUARD(LogExpression) \
		{\
			std::lock_guard<std::mutex> lock(LogMutex);\
			LogExpression;\
		}

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
				bool bHasPendingConnection = false;
				if (Socket->WaitForPendingConnection(bHasPendingConnection, 10000))
				{
					if (bHasPendingConnection)
					{
						LOG_GUARD(std::cout << "server accept start" << std::endl);
						XSocket* NewSocket = Socket->Accept();
						if (NewSocket)
						{
							AllClient.push_back(NewSocket);
						}
						LOG_GUARD(std::cout << "server accept end" << std::endl);
					}
					else
					{
						//std::cout << "server wait for connection timeout!" << std::endl;
					}
				}
				else
				{
					LOG_GUARD(std::cout << "wait for pending connection failed!" << std::endl);
				}

				for (XSocket* Client : AllClient)
				{
					if (Client->Wait(ESocketWaitConditions::WaitForRead, 10000))
					{
						int iData;
						int iBytesRead;
						bool bSuccess = Client->Recv((unsigned char*)(&iData), sizeof(iData), iBytesRead, ESocketReceiveFlags::WaitAll);
						if (bSuccess)
						{
							LOG_GUARD(std::cout << "server recv success, iBytesRead: " << iBytesRead << " content: " << iData << std::endl);
						}
						else
						{
							LOG_GUARD(std::cout << "server recv failed, iBytesRead: " << iBytesRead << std::endl);
						}
					}
				}

			}
			LOG_GUARD(std::cout << "server accept failed" << std::endl);
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
				LOG_GUARD(std::cout << "client send " << BytesSent << " bytes, Send content: " << i++ << std::endl);
			}
			else
			{
				LOG_GUARD(std::cout << "client send failed! Send content: " << i << std::endl);
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}