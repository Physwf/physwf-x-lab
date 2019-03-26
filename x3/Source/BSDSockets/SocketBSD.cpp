#include "SocketBSD.h"

bool XSocketBSD::Close()
{
	if (Socket != INVALID_SOCKET)
	{
		int error = closesocket(Socket);
		Socket = INVALID_SOCKET;
		return error == 0;
	}
	return false;
}

bool XSocketBSD::Bind(const XInternetAddr& Addr)
{
	return bind(Socket, (sockaddr*)(XInternetAddrBSD&)Addr, sizeof(sockaddr_in)) == 0;
}

bool XSocketBSD::Connect(const XInternetAddr& Addr)
{
	int iError = connect(Socket, (sockaddr*)(XInternetAddrBSD&)Addr, sizeof(sockaddr_in));

	return iError == 0;
}

bool XSocketBSD::Listen(int MaxBackLog)
{
	return listen(Socket, MaxBackLog) == 0;
}

bool XSocketBSD::WaitForPendingConnection(bool& bHasPendingConnection, int InMS)
{
	bool bHasSucceeded = false;
	bHasPendingConnection = false;
	if (HasState(ESocketBSDParam::HasError) == ESocketBSDReturn::No)
	{
		ESocketBSDReturn State = HasState(ESocketBSDParam::CanRead);

		bHasSucceeded = State != ESocketBSDReturn::EncounteredError;
		bHasPendingConnection = State == ESocketBSDReturn::Yes;
	}
	return bHasSucceeded;
}

bool XSocketBSD::Wait(ESocketWaitConditions::Type Condition, int InMS)
{
	if (Condition == ESocketWaitConditions::WaitForRead || Condition == ESocketWaitConditions::WaitForReadOrWrite)
	{
		if (HasState(ESocketBSDParam::CanRead) == ESocketBSDReturn::Yes)
		{
			return true;
		}
	}

	if (Condition == ESocketWaitConditions::WaitForWrite || Condition == ESocketWaitConditions::WaitForReadOrWrite)
	{
		if (HasState(ESocketBSDParam::CanWrite) == ESocketBSDReturn::Yes)
		{
			return true;
		}
	}

	return false;
}

class XSocket* XSocketBSD::Accept()
{
	SOCKET NewSocket = accept(Socket, NULL, NULL);
	if (NewSocket != INVALID_SOCKET)
	{
		XSocketSubsystemBSD* SocketSubsystemBSD = static_cast<XSocketSubsystemBSD*>(SocketSubsystem);
		return SocketSubsystemBSD->InternalBSDSocketFactory(NewSocket, SocketType);
	}
	return nullptr;
}

bool XSocketBSD::Send(const unsigned char* Data, int iCount, int& BytesSent)
{
	return false;
}

bool XSocketBSD::Recv(unsigned char* Data, int BufferSize, int& BytesRead)
{
	return false;
}

bool XSocketBSD::SetNonBlocking(bool bNonBlocking /*= true*/)
{
	u_long Value = bNonBlocking ? 1 : 0;
#if PLATFORM_WINDOWS
	return ioctlsocket(Socket, FIONBIO, &Value) == 0;
#endif
}

ESocketBSDReturn XSocketBSD::HasState(ESocketBSDParam State, int InMS /*= 0*/)
{
	timeval Time;
	Time.tv_sec = InMS / 1000;
	Time.tv_usec = InMS % 1000;

	fd_set SocketSet;
	FD_ZERO(&SocketSet);
	FD_SET(Socket, &SocketSet);

	int iSelectStatus = 0;
	switch (State)
	{
	case ESocketBSDParam::CanRead:
		select(Socket + 1, &SocketSet, nullptr, nullptr, &Time);
		break;
	case ESocketBSDParam::CanWrite:
		select(Socket + 1, nullptr, &SocketSet, nullptr, &Time);
		break;
	case ESocketBSDParam::HasError:
		select(Socket + 1,  nullptr, nullptr, &SocketSet, &Time);
		break;
	}

	return iSelectStatus > 0 ? ESocketBSDReturn::Yes :
		iSelectStatus == 0 ? ESocketBSDReturn::No : 
		ESocketBSDReturn::EncounteredError;
}
