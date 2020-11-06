#include "DeferredShading.h"
#include "Scene.h"
#include "DepthOnlyRendering.h"
#include "BasePassRendering.h"
#include "ShadowRendering.h"
#include "LightRendering.h"
#include "AtmosphereRendering.h"
#include "RenderTargets.h"

ID3D11Buffer* GlobalConstantBuffer;
char GlobalConstantBufferData[4096];

void InitShading()
{
	GlobalConstantBuffer = CreateConstantBuffer(false,4096);
	memset(GlobalConstantBufferData, 0, sizeof(GlobalConstantBufferData));

	RenderTargets& SceneContex = RenderTargets::Get();
	SceneContex.Allocate();

	InitScene();
	InitPrePass();
	InitBasePass();
	InitShadowPass();
	InitLightPass();
	InitAtomosphereFog();
}

void Render()
{
	UpdateView();
	RenderPrePass();
	RenderBasePass();
	RenderShadowPass();
	RenderLight();
	RenderAtmosphereFog();

	RenderTargets& SceneContex = RenderTargets::Get();
	SceneContex.FinishRendering();
}


