#pragma once

class UActorComponent
{
public:
	void RegisterComponent();
	void UnregisterComponent();

	void RegisterComponentWithWorld(/*UWorld* InWorld*/);

protected:
	virtual void CreateRenderState_Concurrent();
	virtual void DestroyRenderState_Concurrent();
private:
	void ExecuteUnregisterEvents();
	void ExecuteRegisterEvents();
};