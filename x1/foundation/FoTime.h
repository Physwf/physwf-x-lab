#pragma once

#include <sys/time.h>

#include "FoTypes.h"

inline uint32 GetProcessTime()
{
	static struct timeval tv;
	if(!gettimeofday(&tv,0))
	{
		return 0;
	}
	return tv.tv_sec * 1000 + tv.tv_usec;
}

class FoTime
{

};
