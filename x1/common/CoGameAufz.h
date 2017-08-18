#pragma once

#define GAME_FRAME_TIME 30

#include <vector>

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

class CoCmdBuffer
{
public:
	CoCmdBuffer();
	~CoCmdBuffer();

	void 				Push(const void* pData, int iSize);
	void 				Pop();			// move tail one step to the head
	CoGameCmd*			Head();			// top of the buffer
	CoGameCmd*			Tail();			// tail of the buffer
	bool 				Expand();
	int 				Size() const;
	void 				Clear();
private:
	uint8*				m_pBuffer;
	int 				m_iSize;
	int 				m_iHead;
	int 				m_iTail;		//next cmd will be process
	int 				m_iHeadSize;	//head cmd size
	int 				m_iTailSize;	//next cmd size
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
	bool 			IsWatching() { return m_bWatching; }
private:
	CoCmdBuffer 	m_oBuffer;
	uint32		 	m_uiPackIndex;
	//CoPackInfo*		m_aPackInfo[];
		
	uint32 			m_uiFrame;
	uint32			m_uiLastTime;
	uint32 			m_uiAccumTime;

	bool			m_bWatching;
};
