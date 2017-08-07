#pragma once

enum LoGameResult
{
	LGR_NONE = 0,
	LGR_INIT,
	LGR_LOAD,
	LGR_RACING,
	LGR_END,
	LGR_DEL,
	LGR_ERROR,
};


class IGameCall
{
public:
	virtual ~IGameCall() = default;

	virtual char*	GetDumpPath(void) = 0;

	virtual void	OnGameStream(const int iGameID, int iSeat, int iFrame, void* pkData,int iSize, bool bMustSend) = 0;
	virtual void	OnGameRecordPack(const int iGameID, void* pkData, int iSize) = 0;
	virtual void	OnGameErrorRet();
	virtual void	OnGameSrvModelPack(const int iGameID, void* pkData, int iSize) = 0;

	virtual void	OnAddRaceRet(const int iGameID,bool bAddSuc, int iType, int iSrcSrvID) = 0;
	virtual void 	OnPushStreamRet(const int iGameID, int iSrcSrvID, bool bPushRet, int iSize) = 0;
	virtual void	OnRaceNumsRet() = 0;
	virtual void	OnRaceOffsetRet() = 0;
};

class IGameLogic
{
public:
	IGameLogic() = default;
	virtual ~IGameLogic() = default;

	virtual bool	Init(int iThreadCount, IGameCall* pkGameCall, bool bIsServerMode=false, int iMaxCamp=4) = 0;
	virtual void	Finit() = 0;
	virtual void	SaveAufz(int iGameID) = 0;
	virtual int		GetVersion() = 0;

	virtual void	UpdateAus(float fDeltaTime = .0f) = 0;
	virtual void	PlayerLeave(int iGameID,int iSeat) = 0;
	virtual void	PlayerRelink(int iGameID,int iSeat) = 0;

	virtual void	AddGame(int iGameID, int iType, int iMapID, int iSrcSvrID=0,bool bHost=true) = 0;
	virtual void	DelGame(int iGameID);
	
	virtual void	PushStream(int iGameID,int iOffset, void* pkData, int iSize,int iSrcSvrID) = 0;
	virtual void	PushPack(int iGameID, int iPackIndex, void* pkData, int iSize) = 0;
};
