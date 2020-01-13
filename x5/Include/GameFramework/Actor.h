#pragma once

#include "Math/Vector.h"

class UWorld;
class ULevel;

class AActor
{
public:
	AActor(ULevel* InOwner);
	~AActor();

	void PostSpawnActor();

	void SetActorLocation(FVector InLocation);
	FVector GetActorLocation() const;
	void SetActorRotaion(FVector InRotation);
	FVector GetActorRotation() const;
	void SetActorScale(FVector InScale);
	FVector GetActorScale() const;

	virtual UWorld* GetWorld() const;
private:
	FVector Location;
	FVector Rotation;
	FVector Scale;

	class ULevel* Owner;
	class UStaticMeshComponent* Component;
};