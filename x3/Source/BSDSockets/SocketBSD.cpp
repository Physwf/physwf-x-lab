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

void XSocketBSD::Bind(const XInternetAddrBSD& Addr)
{
	return bind(Socket, (sockaddr*)Addr, sizeof(sockaddr_in)) == 0;
}

void XSocketBSD::Connect(const XInternetAddrBSD& Addr)
{

}

bool XSocketBSD::Send(const unsigned char* Data, int iCount, int& BytesSent)
{
	return false;
}

bool XSocketBSD::Recv(unsigned char* Data, int BufferSize, int& BytesRead)
{
	return false;
}
