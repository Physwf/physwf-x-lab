#pragma once

#include "SocketTypes.h"

class XInternetAddr;

class XSocket
{
protected:
	const ESocketType eSocketType;

public:
	XSocket():
		eSocketType(SOCKTYPE_Unkown)
	{ }

	XSocket(ESocketType eType) :
		eSocketType(eType)
	{ }

	virtual ~XSocket()
	{}

	virtual bool Close() = 0;

	virtual bool Bind(const XInternetAddr& InInternetAddr) = 0;

	virtual bool Connect(const XInternetAddr& InInternetAddr) = 0;

	virtual bool Listen(int MaxBackLog) = 0;

	virtual bool Send(const unsigned char* Data, int iCount, int& BytesSent) = 0;

	virtual bool Recv(unsigned char* Data, int BufferSize, int& BytesRead) = 0;

	virtual bool SetNonBlocking(bool bNonBlocking = true) = 0;
};