#pragma once

#include "Sockets.h"
#include "InternetAddrBSD.h"
#include "SocketSubsystemBSD.h"
#include "SocketSubsystemBSDPrivate.h"

class XSocketBSD : public XSocket
{
public:
	XSocketBSD(SOCKET InSocket, ESocketType InSocketType, ISocketSubsystem * InSubsystem):
		XSocket(InSocketType),
		Socket(InSocket),
		SocketSubsystem(InSubsystem)
	{}

	virtual ~XSocketBSD()
	{
		Close();
	}

	virtual bool Close() override;
	virtual bool Bind(const XInternetAddr& Addr) override;
	virtual bool Connect(const XInternetAddr& Addr) override;
	virtual bool Listen(int MaxBackLog) override;
	virtual class XSocket* Accept() override;
	virtual bool Send(const unsigned char* Data, int iCount, int& BytesSent) override;
	virtual bool Recv(unsigned char* Data, int BufferSize, int& BytesRead) override;
	virtual bool SetNonBlocking(bool bNonBlocking = true) override;
protected:
	ESocketType SocketType;
	SOCKET Socket;

	ISocketSubsystem* SocketSubsystem;
};