#pragma once

#include "Math/Vector.h"

class UWorld;

class AActor
{
public:
	AActor();
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

	class ULevel* LevelPrivite;
	class UStaticMeshComponent* Component;
};