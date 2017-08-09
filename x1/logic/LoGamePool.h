#pragma once

#include <stack>
#include <map>

#include "CoGameCmd.h"
#include "CoGameAufz.h"
#include "LoGame.h"
#include "IGameLogic.h"



class LoGameData
{
public:
	LoGameData();
	~LoGameData();

	bool			Init(int iMapID);
	void			Finit();

	void			SetGameID(gameid_t iGameID);
	int				GetGameID();

	bool			PushStream(const void* pData,int iSize);
	bool			PushStream(int iPackIndex,const void* pData, int iSize);

	LoGameResult	Update(IGameCall* pkCall, gameid_t iGameID, float fDeltaTime=.0f);

	bool 			ProcCmd(IGameCall* pkCall, CoGameCmd* pkGameCmd);
	void			ProcServerCmd(IGameCall* pkCall, gameid_t iGameID, CoGameCmd* pkGameCmd);
	
	void			UpdateAus(IGameCall* pkCall, gameid_t iGameID, int iFrame);

	LoGameResult	GetGameResult();
	void			SetGameResult(LoGameResult eResult);
	void			SetHost(bool bHost);

private:
	gameid_t		m_iGameID;
	int 			m_iMapID;
	LoGameResult	m_eGameResult;
	LoGame*			m_pGame;
	IGameCall*		m_pCall;
	bool 			m_bServerMode;

	CoGameAufz		m_kGameAufz;
};

class LoGamePool : public IGameLogic
{
public:
	LoGamePool();
	~LoGamePool();

	virtual bool	Init(int iThreadCount, IGameCall* pGameCall, int iCapaticy=4);
	virtual void	Finit();
	virtual void	SaveAufz(gameid_t iGameID);
	virtual int		GetVersion();

	virtual void	UpdateAus(float fDeltaTime = .0f);
	virtual void 	PlayerLeave(gameid_t iGameID, int iSeat);
	virtual void 	PlayerRelink(gameid_t iGameID, int iSeat);

	virtual void	AddGame(gameid_t iGameID, int iType, int iMapID, int iSrcSvrID=0, bool bHost=true);
	virtual void 	DelGame(gameid_t iGameID);

	virtual void	PushStream(gameid_t iGameID, int iOffset, const void* pkData, int iSize, int iSrcSvrID);
	virtual void	PushPack(gameid_t iGameID, int iPackIndex, const void* pData, int iSize);
private:
	const 	LoGameData*		GetGameDataByID(gameid_t iGameID) const;
	LoGameData*				GetGameDataByID(gameid_t iGameID);
private:
	LoGameData* 	m_pData;
	IGameCall*		m_pGameCall;
	int 			m_iCapacity;
	std::stack<int> m_oFreeData;
	std::map<gameid_t,int> m_oGameIDMap;
};


