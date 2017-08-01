#pragma once

enum LoGameResult
{
	BRR_NONE = 0,
	BRR_INIT,
	BRR_LOAD,
	BRR_RACING,
	BRR_END,
	BRR_DEL,
	BRR_ERROR,
};


class IGameCall
{
public:
	virtual ~LoGameCall() = default;

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
	IRaceLogic();
	virtual ~IRaceLogic() = default;

	virtual bool	Init();
	virtual void	Finit();
	virtual void	SaveAufz();
	virtual int	GetVersion();

	virtual void	UpdateAus(float fDeltaTime = .0f) = 0;
	virtual void	PlayerLeave(int iGameID,int iSeat) = 0;
	virtual void	PlayerRelink(int iGameID,int iSeat) = 0;

	virtual void	AddRace(int iGameID, int iType, int iMapID, int SrcSrvID=0,bool bHost=true) = 0;
	virtual void	DelRace(int iGameID);
	
	virtual void	PushStream(int iGameID,int iOffset, void* pkData, int iSize,int iSrcSrvID) = 0;
	virtual void	PushPack(int iGameID, int iPackIndex, void* pkData, int iSize) = 0;
};
