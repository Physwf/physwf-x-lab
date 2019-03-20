#pragma once

#include "SoketTypes.h"
#include "InternetAddrBSD.h"

class XSocketBSD
{
public:
	XSocketBSD(SOCKET InSocket, ESocketType InSocketType):
		SocketType(InSocketType),
		Socket(InSocket) 
	{}

	~XSocketBSD()
	{
		Close();
	}

	virtual bool Close();
	virtual bool Bind(const XInternetAddrBSD& Addr);
	virtual bool Connect(const XInternetAddrBSD& Addr);
	virtual bool Listen(int MaxBackLog);
	virtual bool Send(const unsigned char* Data, int iCount, int& BytesSent);
	virtual bool Recv(unsigned char* Data, int BufferSize, int& BytesRead);
	virtual bool SetNonBlocking(bool bNonBlocking = true);
protected:
	ESocketType SocketType;
	SOCKET Socket;
};