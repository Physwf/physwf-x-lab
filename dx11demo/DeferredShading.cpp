#include "DeferredShading.h"
#include "Scene.h"
#include "DepthOnlyRendering.h"
#include "BasePassRendering.h"
#include "ShadowRendering.h"
#include "LightRendering.h"
#include "AtmosphereRendering.h"

ID3D11Buffer* GlobalConstantBuffer;
char GlobalConstantBufferData[4096];

void InitShading()
{
	GlobalConstantBuffer = CreateConstantBuffer(false,4096);
	memset(GlobalConstantBufferData, 0, sizeof(GlobalConstantBufferData));

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
}


