#pragma once

#include <vector>

class AActor;

class ULevel
{
public:
	std::vector<AActor*> Actors;

	class UWorld* OwningWorld;
};