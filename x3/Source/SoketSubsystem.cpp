#include "SocketSubsystem.h"
#include "SocketSubsystemModule.h"

extern void CreateSocketSubsystem(XSocketSubstemModule& SocketSubsystemModule);
extern void DestroySocketSubsystem(XSocketSubstemModule& SocketSubsystemModule);

ISocketSubsystem* ISocketSubsystem::Get()
{
	static XSocketSubstemModule SocketModule;
	SocketModule.StartupModule();
	return SocketModule.GetSocketSubsystem();
}

class ISocketSubsystem* XSocketSubstemModule::GetSocketSubsystem()
{
	return SocketSubsystem;
}

void XSocketSubstemModule::RegisterSocketSubsystem(class ISocketSubsystem* InSubsystem)
{
	SocketSubsystem = InSubsystem;
}

void XSocketSubstemModule::UnregisterSocketSubsystem()
{
	SocketSubsystem = nullptr;
}

void XSocketSubstemModule::StartupModule()
{
	CreateSocketSubsystem(*this);
}

void XSocketSubstemModule::ShutdownModule()
{
	ShutdownSocketSubsystem();
}

void XSocketSubstemModule::ShutdownSocketSubsystem()
{
	DestroySocketSubsystem(*this);
}
