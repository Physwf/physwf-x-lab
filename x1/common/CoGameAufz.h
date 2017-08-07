#pragma once

#include "CoGameCmd.h"

struct SeAufzHead
{
	char 			acFileType[4];
	unsigned int 	uiLogicVersion;
	char 			acClientVersion[4];
	
	int 			iSaveTime;
	int 			iGameTime;
	int 			iMapID;

	unsigned int 	iSize;
	char 			acData[1];

	SeAufzHead()
	{

	}
};

class CoGameAufz
{
public:
	CoGameAufz();
	~CoGameAufz();

	void 			Update(unsigned int iCurTime, unsigned int iLimitTime);

	bool 			PushCmd(const void* pData,int iSize);
	bool 			PushData(const void* pData, int iSize);
	bool 			PushPack(int iPackIndex, const void* pData,int iSize);

	CoGameCmd* 		GetCmd();
	void 			PopCmd();
	
	unsigned int 	GetCurrentFrame();

private:
	int			 	m_iBufferSize;
	unsigned char*	m_pucBuffer;

	unsigned int 	m_uiPackIndex;
	//CoPackInfo*		m_aPackInfo[];
	
	unsigned int 	m_uiFrame;

};
