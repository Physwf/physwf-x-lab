#pragma once

class UWorld;

class UActorComponent
{
public:
	void RegisterComponent();
	void UnregisterComponent();

	void RegisterComponentWithWorld(/*UWorld* InWorld*/);

	UWorld* GetWorld() const  { return WorldPrivate; }
protected:
	virtual void CreateRenderState_Concurrent();
	virtual void DestroyRenderState_Concurrent();
private:
	void ExecuteUnregisterEvents();
	void ExecuteRegisterEvents();

private:
	class UWorld* WorldPrivate;
};