#pragma once


#include "IPAddress.h"
#include "SocketSubsystemBSDPrivate.h"

#include <stdlib.h>
#include <memory.h>


class XInternetAddrBSD : public XInternetAddr
{
protected:
	sockaddr_in Addr;

public:
	XInternetAddrBSD()
	{
		memset(&Addr, 0, sizeof(sockaddr_in));
		Addr.sin_family = AF_INET;
	}

	virtual void SetIp(unsigned int InAddr) override
	{
		Addr.sin_addr.s_addr = htonl(InAddr);
	}

	virtual void SetIp(const in_addr& IpAddr)
	{
		Addr.sin_addr = IpAddr;
	}

	virtual void SetIp(const char* InAddr,bool& bValid) override
	{
		if (InAddr == nullptr)
		{
			bValid = false;
			return;
		}

		auto iLen = strlen(InAddr);
		auto iColonPos = 0;
		for (decltype(iLen) i = 0; i < iLen; ++i)
		{
			if (InAddr[i] == ':')
			{
				iColonPos = i;
				break;
			}
		}

		char IpPart[32] = { 0 };
		char PortPart[8] = { 0 };
		int Port = 0;
		if (iColonPos > 0)
		{
			memcpy_s(IpPart, sizeof(IpPart), InAddr, iColonPos);
			memcpy_s(PortPart,sizeof(PortPart), InAddr + iColonPos + 1, iLen - iColonPos);
			Port = atoi(PortPart);
		}
		else
		{
			memcpy_s(IpPart, sizeof(IpPart), InAddr, iLen);
		}

		in_addr IPv4Addr;
		if (inet_pton(AF_INET, IpPart, &IPv4Addr))
		{
			if (Port != 0)
			{
				SetPort(Port);
			}

			bValid = true;

			SetIp(IPv4Addr);
		}
		else
		{
			bValid = false;
		}
	}

	virtual void GetIp(unsigned int& OutIp) const override
	{
		OutIp = Addr.sin_addr.s_addr;
	}

	virtual void SetPort(unsigned short InPort)
	{
		Addr.sin_port = htons(InPort);
	}

	virtual void GetPort(unsigned short& OutPort) const override
	{
		OutPort = Addr.sin_port;
	}

	virtual unsigned short GetPort() const
	{
		return Addr.sin_port;
	}

	virtual void SetAnyAddress()
	{
		SetIp(INADDR_ANY);
		SetPort(0);
	}

	virtual void SetBroadcastAddress() override
	{

	}

	operator sockaddr*(void)
	{
		return (sockaddr*)&Addr;
	}

	operator const sockaddr*(void) const
	{
		return (const sockaddr*)&Addr;
	}
};