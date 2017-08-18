#pragma once

typedef int gameid_t;

enum LoGameState
{
	LGS_NONE,
	LGS_OVER,
	LGS_INIT,
	LGS_PAUSE,
	LGS_RACING,
	LGS_OVERWAIT,
};

class LoGame
{
public:
	void 			SetGameID(gameid_t iGameID) { m_iGameID = iGameID; } 
	LoGameState 	GetState() { return m_eState; }
	void 			SetState(LoGameState eState);
	uint32 			GetFrameCount() { return 0; }
private:
	gameid_t 		m_iGameID;
	LoGameState 	m_eState;
};
