#include "Address.h"
#include <WS2tcpip.h>

using namespace Net;

AddressIPv4::AddressIPv4(const sockaddr_in& addr_in)
{
	mAddr = addr_in.sin_addr.S_un.S_addr;
	mPort = ntohs(addr_in.sin_port);
}


AddressIPv4::AddressIPv4(const char* addr, UINT16 port) : mPort(port)
{
	sockaddr_in temp;
	inet_pton(AF_INET, addr, &temp.sin_addr);
	mAddr = temp.sin_addr.S_un.S_addr;
}

void AddressIPv4::ToSOCKADDR_IN(sockaddr_in& out_addr_in) const
{
	out_addr_in.sin_family = AF_INET;
	out_addr_in.sin_addr.S_un.S_addr = htonl(mAddr);
	out_addr_in.sin_port = htons(mPort);
}

