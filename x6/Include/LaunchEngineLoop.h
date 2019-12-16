#include "CoreTypes.h"

class FEngineLoop
{
public:
	FEngineLoop();

	virtual ~FEngineLoop() { }

	int32 PreInit();

	int32 Init();

	void Exit();

	void Tick();
};

