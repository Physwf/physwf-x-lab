#pragma once

typedef int gameid_t;

class LoGame
{
public:
	void SetGameID(gameid_t iGameID) { m_iGameID = iGameID; } 
private:
	gameid_t m_iGameID;
};
