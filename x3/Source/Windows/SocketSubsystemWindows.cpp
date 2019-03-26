#include "SocketSubsystemWindows.h"
#include "SocketSubsystemModule.h"
#include "BSDSockets/SocketBSD.h"

void CreateSocketSubsystem(XSocketSubstemModule& SocketSubsystemModule)
{
	XSocketSubsystemWindows* WindowsSocketSubsystem = XSocketSubsystemWindows::Create();

	if (WindowsSocketSubsystem->Init())
	{
		SocketSubsystemModule.RegisterSocketSubsystem(WindowsSocketSubsystem);
		return;
	}

	XSocketSubsystemWindows::Destroy();
}

void DestroySocketSubsystem(XSocketSubstemModule& SocketSubsystemModule)
{
	SocketSubsystemModule.UnregisterSocketSubsystem();
	XSocketSubsystemWindows::Destroy();
}

XSocketSubsystemWindows* XSocketSubsystemWindows::Create()
{
	if (!SocketSingleton)
	{
		SocketSingleton = new XSocketSubsystemWindows();
	}
	return SocketSingleton;
}

void XSocketSubsystemWindows::Destroy()
{

}

bool XSocketSubsystemWindows::Init()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		return false;
	}
	return true;
}

void XSocketSubsystemWindows::Shutdown()
{
	WSACleanup();
}

class XSocket* XSocketSubsystemWindows::CreateSocket(int SocketType, bool bForceUDP)
{
	SOCKET Socket = INVALID_SOCKET;
	XSocket* Result = nullptr;


	switch (SocketType)
	{
	case 0:
		Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		Result = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, SOCKTYPE_Datagram) : nullptr;
		break;
	case 1:
		Socket = socket(AF_INET, SOCK_STREAM ,IPPROTO_TCP);
		Result = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, SOCKTYPE_Sreaming) : nullptr;
		break;
	}
	return Result;
}

XSocketSubsystemWindows* XSocketSubsystemWindows::SocketSingleton = nullptr;

