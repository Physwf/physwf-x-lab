#pragma once


class CompositionLighting
{
public:
	void Init();

	void ProcessBeforeBasePass();

	void ProcessAfterBasePass();

	//void ProcessLpvIndirect(FRHICommandListImmediate& RHICmdList, FViewInfo& View);

	void ProcessAfterLighting();
};

extern CompositionLighting GCompositionLighting;