#include "Socket.h"
#include <WS2tcpip.h>

bool addr_human2machine(const char* ipv4, unsigned short port, sockaddr_in* out_addr_in)
{
	if (out_addr_in == nullptr) return false;
	out_addr_in->sin_family = AF_INET;
	inet_pton(AF_INET, ipv4, &out_addr_in->sin_addr);
	out_addr_in->sin_port = htons(port);
}

bool addr_machine2human(const sockaddr_in* addr_in, char* ipv4, size_t ipv4size, unsigned short* port)
{
	if (addr_in == nullptr) return false;
	if (ipv4 == nullptr) return false;
	inet_ntop(AF_INET, addr_in, ipv4, ipv4size);
	if(port) *port = ntohs(addr_in->sin_port);
	return true;
}

using namespace Net;

Socket::Socket() :hSocket(INVALID_SOCKET)
{
}

Socket::Socket(SOCKET InSocket) : hSocket(InSocket)
{
}

Socket::~Socket()
{
}

Socket* Socket::Create(int InAF, int InType, int InProtocal)
{
	SOCKET hSocket = socket(InAF, InType, InProtocal);
	if (hSocket == INVALID_SOCKET) return nullptr;
	else return new Socket(hSocket);
}

void Socket::Close()
{
	if (hSocket != INVALID_SOCKET)
	{
		closesocket(hSocket);
	}
}

bool Socket::Bind(const sockaddr_in* addr_in)
{
	int res = bind(hSocket, (const sockaddr*)addr_in, sizeof(sockaddr_in));
	if (res == SOCKET_ERROR)
	{
		int LastError = WSAGetLastError();
		return false;
	}
	return true;
}

Socket* Socket::Accept()
{
	sockaddr_in peer_addr = {};
	int addr_len = 0;
	SOCKET hNewSocket = accept(hSocket, (sockaddr*)&peer_addr, &addr_len);
	if (hNewSocket == INVALID_SOCKET)
	{
		int LastError = WSAGetLastError();
		return NULL;
	}
	return new Socket(hNewSocket);
}

int Socket::SendTo(void* InData, int InSize, const sockaddr_in* to)
{
	return sendto(hSocket, (const char*)InData, InSize, 0, (sockaddr*)to, sizeof(sockaddr_in));
}

int Socket::RecvFrom(void* OutBuffer, int BufferSize, sockaddr_in* OutFrom)
{
	int addr_len = sizeof(sockaddr_in);
	return recvfrom(hSocket, (char*)OutBuffer, BufferSize, 0, (sockaddr*)OutFrom, &addr_len);
}

bool Socket::IsValid() const
{
	return hSocket == INVALID_SOCKET;
}


