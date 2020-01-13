#pragma once

class UWorld;
class AActor;

class UActorComponent
{
public:
	UActorComponent(AActor* InOwner);

	void RegisterComponent();
	void UnregisterComponent();

	void RegisterComponentWithWorld(UWorld* InWorld);

	UWorld* GetWorld() const  { return WorldPrivate; }

public:
	AActor* GetOwner() const;
protected:
	virtual void CreateRenderState_Concurrent();
	virtual void DestroyRenderState_Concurrent();
private:
	void ExecuteUnregisterEvents();
	void ExecuteRegisterEvents();

private:
	UWorld* WorldPrivate;
	AActor* Owner;
};