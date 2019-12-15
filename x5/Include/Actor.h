#pragma once

#include "Math/Vector.h"

class AActor
{
public:
	AActor();
	~AActor();

	void SetActorLocation(FVector InLocation);
	FVector GetActorLocation() const;
	void SetActorRotaion(FVector InRotation);
	FVector GetActorRotation() const;
	void SetActorScale(FVector InScale);
	FVector GetActorScale() const;
private:
	FVector Location;
	FVector Rotation;
	FVector Scale;
};