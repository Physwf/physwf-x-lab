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

ESocketErrors XSocketSubsystemWindows::GetLastErrorCode()
{
	return TranslateErrorCode(WSAGetLastError());
}

ESocketErrors XSocketSubsystemWindows::TranslateErrorCode(int Code)
{
	if (Code == SOCKET_ERROR)
	{
		return GetLastErrorCode();
	}

	switch (Code)
	{
	case 0: return SE_NO_ERROR;
	case ERROR_INVALID_HANDLE: return SE_ECONNRESET; // invalid socket handle catch
	case WSAEINTR: return SE_EINTR;
	case WSAEBADF: return SE_EBADF;
	case WSAEACCES: return SE_EACCES;
	case WSAEFAULT: return SE_EFAULT;
	case WSAEINVAL: return SE_EINVAL;
	case WSAEMFILE: return SE_EMFILE;
	case WSAEWOULDBLOCK: return SE_EWOULDBLOCK;
	case WSAEINPROGRESS: return SE_EINPROGRESS;
	case WSAEALREADY: return SE_EALREADY;
	case WSAENOTSOCK: return SE_ENOTSOCK;
	case WSAEDESTADDRREQ: return SE_EDESTADDRREQ;
	case WSAEMSGSIZE: return SE_EMSGSIZE;
	case WSAEPROTOTYPE: return SE_EPROTOTYPE;
	case WSAENOPROTOOPT: return SE_ENOPROTOOPT;
	case WSAEPROTONOSUPPORT: return SE_EPROTONOSUPPORT;
	case WSAESOCKTNOSUPPORT: return SE_ESOCKTNOSUPPORT;
	case WSAEOPNOTSUPP: return SE_EOPNOTSUPP;
	case WSAEPFNOSUPPORT: return SE_EPFNOSUPPORT;
	case WSAEAFNOSUPPORT: return SE_EAFNOSUPPORT;
	case WSAEADDRINUSE: return SE_EADDRINUSE;
	case WSAEADDRNOTAVAIL: return SE_EADDRNOTAVAIL;
	case WSAENETDOWN: return SE_ENETDOWN;
	case WSAENETUNREACH: return SE_ENETUNREACH;
	case WSAENETRESET: return SE_ENETRESET;
	case WSAECONNABORTED: return SE_ECONNABORTED;
	case WSAECONNRESET: return SE_ECONNRESET;
	case WSAENOBUFS: return SE_ENOBUFS;
	case WSAEISCONN: return SE_EISCONN;
	case WSAENOTCONN: return SE_ENOTCONN;
	case WSAESHUTDOWN: return SE_ESHUTDOWN;
	case WSAETOOMANYREFS: return SE_ETOOMANYREFS;
	case WSAETIMEDOUT: return SE_ETIMEDOUT;
	case WSAECONNREFUSED: return SE_ECONNREFUSED;
	case WSAELOOP: return SE_ELOOP;
	case WSAENAMETOOLONG: return SE_ENAMETOOLONG;
	case WSAEHOSTDOWN: return SE_EHOSTDOWN;
	case WSAEHOSTUNREACH: return SE_EHOSTUNREACH;
	case WSAENOTEMPTY: return SE_ENOTEMPTY;
	case WSAEPROCLIM: return SE_EPROCLIM;
	case WSAEUSERS: return SE_EUSERS;
	case WSAEDQUOT: return SE_EDQUOT;
	case WSAESTALE: return SE_ESTALE;
	case WSAEREMOTE: return SE_EREMOTE;
	case WSAEDISCON: return SE_EDISCON;
	case WSASYSNOTREADY: return SE_SYSNOTREADY;
	case WSAVERNOTSUPPORTED: return SE_VERNOTSUPPORTED;
	case WSANOTINITIALISED: return SE_NOTINITIALISED;
	case WSAHOST_NOT_FOUND: return SE_HOST_NOT_FOUND;
	case WSATRY_AGAIN: return SE_TRY_AGAIN;
	case WSANO_RECOVERY: return SE_NO_RECOVERY;
	case WSANO_DATA: return SE_NO_DATA;
		// case : return SE_UDP_ERR_PORT_UNREACH; //@TODO Find it's replacement
	}

	return SE_NO_ERROR;
}

XSocketSubsystemWindows* XSocketSubsystemWindows::SocketSingleton = nullptr;

