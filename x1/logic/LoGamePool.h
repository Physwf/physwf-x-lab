#pragma once

#include "LoGame.h"
#include "IGameLogic.h"

class LoGameData
{
public:
	LoGameData();
	~LoGameData();

	bool		Init(int iMapID);
	void		Finit();

	void		SetGameID(int iID);
	int		GetGameID();

	bool		PushStream();
	bool		PushStream();

	LoGameResult	GetGameResult();
	LoGameResult	Update(LoGameCall* pkCall, int iGameID,bool bSrvMode = false, float fDeltaTime=.0f);

	bool 		ProcCmd(LoGameCall* pkCall, CoGameCmd* pkGameCmd);

	void		ProcServerCmd(LoGameCall* pkCall, int iGameID, CoGameCmd* pkGameCmd);
	void		UpdateAus(LoGameCall* pkCall, int iGameID, int iFrame);

	void		SetGameResult(CoGameResult eResult)'
	void		SetHost(bool bHost);

private:
	int		m_iGameID;
	LoGame*		m_pGame;

	CoGameAufz	m_kGameAufz;
};

class LoGamePool : public IGameLogic
{
public:
	LoGamePool();
	~LoGamePool();

	virtual bool	Init();
	virtual void	Finit();

	virtual void	UpdateAus(float fDeltaTime=.0f);

	virtual void	AddGame(int iID,int iType,int iMapID,bool bHost);
	virtual void 	DelGame(int iID);

	virtual void	PushStream(int iID);
	virtual void	PushPack(int iID);
private:
	LoGameData* 	pData;
	int 		iCapacity;
};


