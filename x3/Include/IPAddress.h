#pragma once


class XInternetAddr
{

public:

	virtual void SetIp(unsigned int InAddr) = 0;

	virtual void SetIp(const char* InAddr, bool& bValid) = 0;

	virtual void GetIp(unsigned int& OutIp) const = 0;

	virtual void SetPort(unsigned short InPort) = 0;

	virtual void GetPort(unsigned short& OutPort) const = 0;

	virtual unsigned short GetPort() const = 0;

	virtual void SetAnyAddress() = 0;

	virtual void SetBroadcastAddress() = 0;
};