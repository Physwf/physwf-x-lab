#pragma once

#include <chrono>
#include <stdio.h>
#include <string>

class AutoEclipse
{
public:
	AutoEclipse(const char* cName):Name(cName)
	{
		Start = std::chrono::system_clock::now();
	}
	~AutoEclipse()
	{
		std::chrono::system_clock::duration TimeEclipse = std::chrono::system_clock::now() - Start;
		printf("%s:%lldms", Name.c_str(), TimeEclipse.count()/10000);
	}
private:
	std::chrono::system_clock::time_point Start;
	std::string Name;
};