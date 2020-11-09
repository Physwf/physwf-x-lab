#include "CompositionLighting.h"
#include "RenderTargets.h"
#include "PostProcessAmbientOcclution.h"
#include "SceneOcclusion.h"
#include "GPUProfiler.h"

RCPassPostProcessAmbientOcclusionSetup AOSetup1;
RCPassPostProcessAmbientOcclusionSetup AOSetup2;
RCPassPostProcessAmbientOcclusion AO0;
RCPassPostProcessAmbientOcclusion AO1;
RCPassPostProcessAmbientOcclusion AO2;
RCPassPostProcessBasePassAO BasePassAO;

void CompositionLighting::Init()
{
	int Levels = 2;

	RenderTargets& SceneContext = RenderTargets::Get();

	if (Levels >= 2)
	{
		AOSetup1.Input0 = SceneContext.GetSceneDepthTexture();
		AOSetup1.Input1 = NULL;
		AOSetup1.Init();
	}

	if (Levels >= 3)
	{
		AOSetup2.Input0 = NULL;
		AOSetup2.Input1 = AOSetup1.Output;
		AOSetup2.Init();
	}

	// upsample from lower resolution

	if (Levels >= 3)
	{
		AO2.Input0 = AOSetup2.Output;
		AO2.Input1 = AOSetup2.Output;
		AO2.Input2 = NULL;
		//AO2.Input3 = HZBSRVs[0];
		AO2.Init();
	}

	if (Levels >= 2)
	{
		AO1.Input0 = AOSetup1.Output;
		AO1.Input1 = AOSetup1.Output;
		AO1.Input2 = AO2.Output;
		//AO1.Input3 = HZBSRVs[0];
		AO1.Init();
	}

	AO0.Input0 = SceneContext.GetGBufferATexture();
	AO0.Input1 = AOSetup1.Output;
	AO0.Input2 = AO1.Output;
	//AO0.Input3 = HZBSRVs[0];
	AO0.Init();
}

void CompositionLighting::ProcessBeforeBasePass(ViewInfo& View)
{
	
}

void CompositionLighting::ProcessAfterBasePass(ViewInfo& View)
{
	SCOPED_DRAW_EVENT_FORMAT(ProcessAfterBasePass,TEXT("ProcessAfterBasePass"));
	int Levels = 2;

	if (Levels >= 3)
	{
		AOSetup2.Process(View);
	}
	if (Levels >= 2)
	{
		AOSetup1.Process(View);
	}

	// upsample from lower resolution
	if (Levels >= 3)
	{
		AO2.Process(View);
	}
	if (Levels >= 2)
	{
		AO1.Process(View);
	}

	AO0.Process(View);
}

void CompositionLighting::ProcessAfterLighting(ViewInfo& View)
{

}

CompositionLighting GCompositionLighting;