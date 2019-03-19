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
	virtual void Bind(const XInternetAddrBSD& Addr);
	virtual void Connect(const XInternetAddrBSD& Addr);
	virtual bool Send(const unsigned char* Data, int iCount, int& BytesSent);
	virtual bool Recv(unsigned char* Data, int BufferSize, int& BytesRead);

protected:
	ESocketType SocketType;
	SOCKET Socket;
};