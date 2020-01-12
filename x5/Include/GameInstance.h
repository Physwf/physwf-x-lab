#pragma once

class ULocalPlayer;

class UGameInstance
{
protected:
	struct FWorldContext* WorldContext;
	class UEngine* Outer;
public:
	UGameInstance(class UEngine* InOuter) 
		:Outer(InOuter)
	{}
public:
	void InitializeStandalone();

	virtual void Init();

	virtual void StartGameInstance();

	class UWorld* GetWorld() const;
	class UEngine* GetEngine() const;
};