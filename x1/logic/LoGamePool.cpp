#include "LoGamePool.h"

LoGameData::LoGameData()
{
	m_iGameID = 0;
	m_iMapID = 0;
	m_eGameResult = LGR_NONE;
	m_pGame = nullptr;
	m_pCall = nullptr;
	m_bServerMode = false;
}

LoGameData::~LoGameData()
{
}

bool LoGameData::Init(int iMapID)
{
	if(LGR_NONE == m_eGameResult)
	{
		m_eGameResult = LGR_INIT;
		m_iMapID = iMapID;
		//todo poolize
		m_pGame = new LoGame();
		return true;
	}
	return false;
}

void LoGameData::SetGameID(gameid_t iGameID)
{
	m_iGameID = iGameID;
	m_pGame->SetGameID(iGameID);
}

bool LoGameData::PushStream(const void* pData, int iSize)
{
	return false;
}

bool LoGameData::PushStream(int iPackIndex, const void* pData, int iSize)
{
	return false;
}

LoGameResult LoGameData::GetGameResult()
{
	return m_eGameResult;
}

void LoGameData::SetGameResult(LoGameResult eResult)
{
}

bool LoGameData::ProcCmd(IGameCall* pkCall, CoGameCmd* pkGameCmd)
{
	return false;
}

void LoGameData::UpdateAus(IGameCall* pkCall, gameid_t iGameID, int iFrame)
{
}

LoGameResult LoGameData::Update(IGameCall* pkCall, gameid_t iGameID,float fDeltaTime/*=.0f*/)
{
	return LGR_NONE;
}

void LoGameData::ProcServerCmd(IGameCall* pkCall, gameid_t iGameID, CoGameCmd* pkGameCmd)
{
}

void LoGameData::SetHost(bool bHost)
{
}

LoGamePool::LoGamePool()
{
}

LoGamePool::~LoGamePool()
{
}

bool LoGamePool::Init(int iThreadCount, IGameCall* pGameCall, int iCapaticy/*=4*/)
{
	m_iCapacity = iCapaticy;
	m_pGameCall = pGameCall;
	//todo poolize
	m_pData = new LoGameData[m_iCapacity];

	for(int i=0;i<iCapaticy;++i)
	{
		m_FreeData.push(i);
	}

	return false;
}

void LoGamePool::Finit()
{
}

void LoGamePool::SaveAufz(gameid_t iGameID)
{
}

int LoGamePool::GetVersion()
{
	return 0;
}

void LoGamePool::UpdateAus(float fDeltaTime/*=.0f*/)
{
}

void LoGamePool::PlayerLeave(gameid_t iGameID,int iSeat)
{
}

void LoGamePool::PlayerRelink(gameid_t iGameID, int iSeat)
{
}

void LoGamePool::AddGame(gameid_t iGameID, int iType,int iMapID, int iSrcSvrID/*=0*/,bool bHost/*=true*/)
{
	bool bAddSuc =  true;
	if(m_FreeData.empty())
	{
		bAddSuc = false;
	}
	else
	{
		int iIndex = m_FreeData.top();
		LoGameData& rData =  m_pData[iIndex];

		if(rData.Init(iMapID))
		{
			rData.SetGameID(iGameID);
			rData.SetHost(bHost);
			if(bHost)
			{
				//rData.OnHostStart();
			}
			m_FreeData.pop();
			m_GameIDMap.insert(std::make_pair(iGameID,iIndex));
		}
		else
		{
			rData.SetGameResult(LGR_DEL);
		}
	}
	
	m_pGameCall->OnAddGameRet(iGameID,bAddSuc,iType,iSrcSvrID);
}

void LoGamePool::DelGame(gameid_t iGameID)
{
}

void LoGamePool::PushStream(gameid_t iGameID, int iOffset, const void* pData, int iSize, int iSrcSvrID)
{
}

void LoGamePool::PushPack(gameid_t iGameID, int iPackIndex, const void* pData, int iSize)
{
}


