#pragma once

#include <vector>

class AActor;
class UWorld;

class ULevel
{
public:
	ULevel(UWorld* InOwner);

	std::vector<AActor*> Actors;

	class UWorld* OwningWorld;
};