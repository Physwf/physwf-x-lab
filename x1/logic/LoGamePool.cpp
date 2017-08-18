#include "FoTime.h"

#include "LoGamePool.h"

LoGameData::LoGameData()
{
	m_iGameID = 0;
	m_iMapID = 0;
	m_eGameResult = LGR_NONE;
	m_pGame = nullptr;
	m_pCall = nullptr;
	m_bServerMode = false;
	m_fLastUpdateTime = 0.0f;
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

gameid_t LoGameData::GetGameID()
{
	return m_iGameID;
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
	m_eGameResult = eResult;
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
	if(fDeltaTime != 0.0f)
	{
		m_fLastUpdateTime += fDeltaTime;
	}
	else
	{
		m_fLastUpdateTime = GetProcessTime();
	}

	if(m_pGame->GetState() == LGS_RACING && m_oGameAufz.IsWatching() )
	{
		m_oGameAufz.Update(m_fLastUpdateTime,5*GAME_FRAME_TIME);
	}

	CoGameCmd* pCmd = m_oGameAufz.GetCmd();
	if(pCmd && pCmd->uiFrame == 0)
	{
		ProcCmd(m_pCall,pCmd);
	}
	
	uint32 uiCurrentFrame = m_oGameAufz.GetCurrentFrame();
	while(m_pGame->GetFrameCount() < uiCurrentFrame && m_pGame->GetState() != LGS_OVER)
	{

	}

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
		m_oFreeData.push(i);
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
	for(int i = 0; i < m_iCapacity; ++i)
	{
		LoGameData* pGame = &(m_pData[i]);
		LoGameResult eResult = pGame->GetGameResult();
		gameid_t iGameID = pGame->GetGameID();
		
		if(LGR_INIT == eResult || LGR_LOAD == eResult || eResult == LGR_RACING)
		{
			pGame->UpdateAus(m_pGameCall,iGameID,fDeltaTime);
		}

		eResult = pGame->GetGameResult();
		if(LGR_END == eResult)
		{
			DelGame(iGameID);
			pGame->SetGameResult(LGR_DEL);
		}
		else if(LGR_ERROR == eResult)
		{
			pGame->SetGameResult(LGR_DEL);
		}
	}

	for(int i = 0; i< m_iCapacity; ++i)
	{
		LoGameData* pGame = &(m_pData[i]);
		if(LGR_DEL == pGame->GetGameResult())
		{
			//todo:send last data
			m_oFreeData.push(i);
			m_oGameIDMap.erase(pGame->GetGameID());
		}
	}
}

void LoGamePool::PlayerLeave(gameid_t iGameID,int iSeat)
{
	LoGameData* pGame = GetGameDataByID(iGameID);

	if(pGame && pGame->GetGameResult() != LGR_ERROR)
	{
		//pGame->SetPlayerLinkState();
	}
}

void LoGamePool::PlayerRelink(gameid_t iGameID, int iSeat)
{
	LoGameData* pGame = GetGameDataByID(iGameID);

	if(pGame && pGame->GetGameResult() != LGR_ERROR)
	{
		//pGame->SetPlayerLinkState();
	}
}

void LoGamePool::AddGame(gameid_t iGameID, int iType,int iMapID, int iSrcSvrID/*=0*/,bool bHost/*=true*/)
{
	bool bAddSuc =  true;
	if(m_oFreeData.empty())
	{
		bAddSuc = false;
	}
	else
	{
		int iIndex = m_oFreeData.top();
		LoGameData& rData =  m_pData[iIndex];

		if(rData.Init(iMapID))
		{
			rData.SetGameID(iGameID);
			rData.SetHost(bHost);
			if(bHost)
			{
				//rData.OnHostStart();
			}
			m_oFreeData.pop();
			m_oGameIDMap.insert(std::make_pair(iGameID,iIndex));
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
	LoGameData* pGame = GetGameDataByID(iGameID);

	if(pGame && pGame->GetGameResult() != LGR_ERROR)
	{
		pGame->SetGameResult(LGR_DEL);
	}
}

void LoGamePool::PushStream(gameid_t iGameID, int iOffset, const void* pData, int iSize, int iSrcSvrID)
{
	LoGameData* pGame = GetGameDataByID(iGameID);

	if(pGame && pGame->GetGameResult() != LGR_ERROR)
	{
		pGame->PushStream(pData,iSize);
	}
}

void LoGamePool::PushPack(gameid_t iGameID, int iPackIndex, const void* pData, int iSize)
{
	LoGameData* pGame = GetGameDataByID(iGameID);

	if(pGame && pGame->GetGameResult() != LGR_ERROR)
	{
		pGame->PushStream(iPackIndex,pData,iSize);
	}
}

const LoGameData* LoGamePool::GetGameDataByID(gameid_t iGameID) const
{
	auto itResult = m_oGameIDMap.find(iGameID);
	if(itResult != m_oGameIDMap.end())
	{
		int iIndex = itResult->second;
		return &m_pData[iIndex];
	}
	return nullptr;
}

LoGameData* LoGamePool::GetGameDataByID(gameid_t iGameID)
{
	auto itResult = m_oGameIDMap.find(iGameID);
	if(itResult != m_oGameIDMap.end())
	{
		int iIndex = itResult->second;
		return &m_pData[iIndex];
	}
	return nullptr;
}
