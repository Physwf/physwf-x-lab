#pragma once

class CoGameAufz
{
public:
	CoGameAufz();
	~CoGameAufz();

	void Update();

	bool PushCmd();
	bool PushData();
	bool PushPack();

	CoGameCmd* GetCmd();
	
	unsigned int GetCurrentFrame();

};
