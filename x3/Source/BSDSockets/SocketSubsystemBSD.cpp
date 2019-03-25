#include "SocketSubsystemBSD.h"

class XSocket* XSocketSubsystemBSD::CreateSocket(bool bForceUDP /* = false */)
{
	return nullptr;
}

void XSocketSubsystemBSD::DestroySocket(class XSocket* InSocket)
{

}
