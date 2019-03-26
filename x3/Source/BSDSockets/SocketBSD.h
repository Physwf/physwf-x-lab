#pragma once

#include "Sockets.h"
#include "InternetAddrBSD.h"
#include "SocketSubsystemBSD.h"
#include "SocketSubsystemBSDPrivate.h"

enum class ESocketBSDParam
{
	CanRead,
	CanWrite,
	HasError,
};

enum class ESocketBSDReturn
{
	Yes,
	No,
	EncounteredError,
};

class XSocketBSD : public XSocket
{

public:
	SOCKET GetNativeSocket() { return Socket; }
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
	virtual bool WaitForPendingConnection(bool& bHasPendingConnection, int InMS) override;
	virtual bool Wait(ESocketWaitConditions::Type Condition, int InMS) override;
	virtual class XSocket* Accept() override;
	virtual bool Send(const unsigned char* Data, int iCount, int& BytesSent) override;
	virtual bool Recv(unsigned char* Data, int BufferSize, int& BytesRead) override;
	virtual bool SetNonBlocking(bool bNonBlocking = true) override;

protected:

	virtual ESocketBSDReturn HasState(ESocketBSDParam State, int InMS = 0);

	ESocketType SocketType;
	SOCKET Socket;

	ISocketSubsystem* SocketSubsystem;
};