#include "SocketBSD.h"

inline int TranslateFlags(ESocketReceiveFlags::Type Flags)
{
	int iTranslateFlags = 0;

	if (Flags & ESocketReceiveFlags::Peek)
	{
		iTranslateFlags |= MSG_PEEK;
#if PLATFORM_HAS_BSD_SOCKET_FEATURE_MSG_DONTWAIT
		TranslatedFlags |= MSG_DONTWAIT;
#endif // PLATFORM_HAS_BSD_SOCKET_FEATURE_MSG_DONTWAIT
	}

	if (Flags & ESocketReceiveFlags::WaitAll)
	{
		iTranslateFlags |= MSG_WAITALL;
	}

	return iTranslateFlags;
}

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
		ESocketBSDReturn State = HasState(ESocketBSDParam::CanRead, InMS);

		bHasSucceeded = State != ESocketBSDReturn::EncounteredError;
		bHasPendingConnection = State == ESocketBSDReturn::Yes;
	}
	return bHasSucceeded;
}

bool XSocketBSD::Wait(ESocketWaitConditions::Type Condition, int InMS)
{
	if (Condition == ESocketWaitConditions::WaitForRead || Condition == ESocketWaitConditions::WaitForReadOrWrite)
	{
		if (HasState(ESocketBSDParam::CanRead, InMS) == ESocketBSDReturn::Yes)
		{
			return true;
		}
	}

	if (Condition == ESocketWaitConditions::WaitForWrite || Condition == ESocketWaitConditions::WaitForReadOrWrite)
	{
		if (HasState(ESocketBSDParam::CanWrite, InMS) == ESocketBSDReturn::Yes)
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
		return SocketSubsystemBSD->InternalBSDSocketFactory(NewSocket, eSocketType);
	}
	return nullptr;
}

bool XSocketBSD::Send(const unsigned char* Data, int iCount, int& BytesSent)
{
	BytesSent = send(Socket, (const char*)(Data), iCount, 0);

	bool bSuccess = BytesSent >= 0;

	return bSuccess;
}

bool XSocketBSD::Recv(unsigned char* Data, int BufferSize, int& BytesRead, ESocketReceiveFlags::Type Flags/* = ESocketReceiveFlags::None*/)
{
	bool bIsStreamSocket = eSocketType == SOCKTYPE_Sreaming;
	int iTranslateFlags = TranslateFlags(Flags);
	bool bSuccess = false;

	BytesRead = recv(Socket, (char*)Data, BufferSize, iTranslateFlags);

	if (BytesRead >= 0)
	{
		bSuccess = !bIsStreamSocket || BytesRead > 0;
	}
	else
	{
		bSuccess = bIsStreamSocket && (SocketSubsystem->TranslateErrorCode(BytesRead) == SE_EWOULDBLOCK);
		BytesRead = 0;
	}
	return bSuccess;
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
	Time.tv_sec = InMS / 1000000;
	Time.tv_usec = InMS % 1000000;

	fd_set SocketSet;
	FD_ZERO(&SocketSet);
	FD_SET(Socket, &SocketSet);

	int iSelectStatus = 0;
	switch (State)
	{
	case ESocketBSDParam::CanRead:
		iSelectStatus = select(Socket + 1, &SocketSet, nullptr, nullptr, &Time);
		break;
	case ESocketBSDParam::CanWrite:
		iSelectStatus = select(Socket + 1, nullptr, &SocketSet, nullptr, &Time);
		break;
	case ESocketBSDParam::HasError:
		iSelectStatus = select(Socket + 1,  nullptr, nullptr, &SocketSet, &Time);
		break;
	}

	return iSelectStatus > 0 ? ESocketBSDReturn::Yes :
		iSelectStatus == 0 ? ESocketBSDReturn::No : 
		ESocketBSDReturn::EncounteredError;
}
