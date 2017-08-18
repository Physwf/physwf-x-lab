#include <string.h>

#include "CoGameAufz.h"

/**
 * CoCmdBuffer
 * @author Joe
 */
CoCmdBuffer::CoCmdBuffer():m_iSize(0),m_iHead(0),m_iTail(0),m_iHeadSize(0),m_iTailSize(0)
{

}

CoCmdBuffer::~CoCmdBuffer()
{
	if(!m_pBuffer)
		delete[] m_pBuffer;
}

void CoCmdBuffer::Push(const void* pData, int iSize)
{
	if(m_iHead + iSize > m_iSize)
	{
		Expand();
	}

	memcpy(m_pBuffer+m_iHead, pData, iSize);
	m_iHead += iSize;

	m_iHeadSize = iSize;
}

void CoCmdBuffer::Pop()
{
	m_iTail += m_iTailSize;
}

CoGameCmd* CoCmdBuffer::Head()
{
	return (CoGameCmd*)(m_pBuffer - m_iHeadSize); 
}

CoGameCmd* CoCmdBuffer::Tail()
{
	CoGameCmd* pTail = (CoGameCmd*)(m_pBuffer + m_iTail);
	m_iTailSize = GetCmdSize(pTail, m_iHead - m_iTail);
	return pTail;
}

bool CoCmdBuffer::Expand()
{
	const int iDeltaSize = 4096 * 10;

	uint8* pNewBuffer = new uint8[m_iSize + iDeltaSize];
	delete[] m_pBuffer;

	m_iSize += iDeltaSize;
	m_pBuffer = pNewBuffer;
	return true;
}

int CoCmdBuffer::Size() const
{
	return m_iSize;
}

void CoCmdBuffer::Clear()
{
	m_iSize = 0;
	m_iHead = 0;
	m_iTail = 0;
	m_iHeadSize = 0;
	m_iTailSize = 0;
}

/**
 * CoGameAufz
 * @author Joe
 */
CoGameAufz::CoGameAufz()
{

}

CoGameAufz::~CoGameAufz()
{
}

void CoGameAufz::Update(unsigned int iCurTime, unsigned int iLimitTime)
{
	uint32 uiDeltaTime = iCurTime - m_uiLastTime;
	if(uiDeltaTime > iLimitTime)
	{
		uiDeltaTime = iLimitTime;
	}
	
	m_uiAccumTime += uiDeltaTime;

	if(uiDeltaTime >= GAME_FRAME_TIME)
	{
		uint32 uiDeltaFrame = m_uiAccumTime / GAME_FRAME_TIME;
		m_uiFrame += uiDeltaTime;
		m_uiAccumTime -= uiDeltaTime*GAME_FRAME_TIME;
	}

	m_uiLastTime = iCurTime;
}

bool CoGameAufz::PushCmd(const void* pData, int iSize)
{
	//todo: verify this in upstream
	if(iSize < (int)sizeof(CoGameCmd)) return false;
	//todo: sync mechanism
	//if(!m_bHostServer) return false;
	//check command size
	int iCmdSize = GetCmdSize(pData,iSize);
	if(iCmdSize != iSize) return false;
	//copy to the buffer
	m_oBuffer.Push(pData,iSize);
	//set frame num
	CoGameCmd* pCmd = (CoGameCmd*)m_oBuffer.Head();
	if(pCmd->uiCmd != CGC_FRAME)
	{
		pCmd->uiFrame = m_uiFrame;
	}
	//todo: process cmd
	return true;
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
	return m_oBuffer.Tail();
}

void CoGameAufz::PopCmd()
{
	m_oBuffer.Pop();
}

unsigned int CoGameAufz::GetCurrentFrame()
{
	return 0;
}

