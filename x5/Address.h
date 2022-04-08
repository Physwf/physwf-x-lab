#pragma once

#ifdef WIN32
#include <WinSock2.h>
#else
#endif

namespace Net
{
	class AddressIPv4
	{
	public:
		AddressIPv4() {}
		AddressIPv4(const sockaddr_in& addr_in);
		AddressIPv4(int addr, UINT16 port) :mAddr(addr), mPort(port) { }
		AddressIPv4(const char* addr, UINT16 port);

		void ToSOCKADDR_IN(sockaddr_in& out_addr_in) const;

	private:
		ULONG mAddr;
		UINT16 mPort;
	};
}