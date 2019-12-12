#pragma once

class XSocketSubstemModule 
{
private:
	class ISocketSubsystem* SocketSubsystem;

	virtual void ShutdownSocketSubsystem();
public:

	virtual class ISocketSubsystem* GetSocketSubsystem();

	virtual void RegisterSocketSubsystem(class ISocketSubsystem* InSubsystem);

	virtual void UnregisterSocketSubsystem();

	virtual void StartupModule();

	virtual void ShutdownModule();

};


