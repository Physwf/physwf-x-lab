#include "CoGameAufz.h"

CoGameAufz::CoGameAufz()
{
}

CoGameAufz::~CoGameAufz()
{
}

void CoGameAufz::Update(unsigned int iCurTime, unsigned int iLimitTime)
{
}

bool CoGameAufz::PushCmd(const void* pData, int iSize)
{
	return false;
}

bool CoGameAufz::PushData(const void* pData, int iSize)
{
	return false;
}

bool CoGameAufz::PushPack(int iPackIndex, const void* pData, int iSize)
{
	return false;
}

CoGameCmd* CoGameAufz::GetCmd()
{
	return 0;
}

void CoGameAufz::PopCmd()
{
}

unsigned int CoGameAufz::GetCurrentFrame()
{
	return 0;
}


