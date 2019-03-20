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

bool XSocketBSD::Bind(const XInternetAddrBSD& Addr)
{
	return bind(Socket, (sockaddr*)&Addr, sizeof(sockaddr_in)) == 0;
}

bool XSocketBSD::Connect(const XInternetAddrBSD& Addr)
{
	int iError = connect(Socket, (sockaddr*)&Addr, sizeof(sockaddr_in));

	return false;
}

bool XSocketBSD::Listen(int MaxBackLog)
{
	return listen(Socket, MaxBackLog) == 0;
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
