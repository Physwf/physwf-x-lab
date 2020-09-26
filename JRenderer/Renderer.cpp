#include "Renderer.h"

MeshDrawingPolicy::MeshDrawingPolicy(const VertexFactory* InVF) :VF(InVF)
{

}

void MeshDrawingPolicy::SetMeshRenderState(ID3D11DeviceContext* Context)
{

}

void MeshDrawingPolicy::DrawMesh(ID3D11DeviceContext* Context)
{

}

void MeshDrawingPolicy::SetupPipelineState()
{

}

void MeshDrawingPolicy::SetShaderState(ID3D11DeviceContext* Context)
{

}

const VertexDeclaration* MeshDrawingPolicy::GetVertexDeclaration() const
{

}

DepthDrawingPolicy::DepthDrawingPolicy(const FVertexFactory* InVF)
{

}

void DepthDrawingPolicy::SetSharedState(ID3D11DeviceContext* Context)
{

	MeshDrawingPolicy::SetShaderState(Context);
}

SceneRenderer::SceneRenderer(const SceneView* View)
{

}

SceneRenderer::~SceneRenderer()
{

}

SceneRenderer* SceneRenderer::CreateSceneRenderer()
{

}

DeferredShadingSceneRenderer::DeferredShadingSceneRenderer(const SceneView* View)
	: SceneRenderer(View)
{

}

bool DeferredShadingSceneRenderer::InitViews(ID3D11DeviceContext* Context)
{

}

bool DeferredShadingSceneRenderer::PreRenderPrePass(ID3D11DeviceContext* Context)
{
	SceneRenderTargets& SceneContext = SceneRenderTargets::Get();
	SceneContext.BeginRenderingPrePass();
	
}

bool DeferredShadingSceneRenderer::RenderPrePassView(ID3D11DeviceContext* Context, const ViewInfo& InView)
{
	//SetupPrePassView(Context, View, this);
	//if (!View.IsInstancedStereoPass())
	{

	}
// 	else
// 	{
// 
// 	}
}

bool DeferredShadingSceneRenderer::RenderPrePass(ID3D11DeviceContext* Context)
{
	bool bDepthWasCleared = false;

	bDepthWasCleared = PreRenderPrePass(Context);

	bool bDirty = false;
	bool bDidPrePre = false;
	bDirty |= RenderBasePassView(Context, View);

	SceneRenderTargets& SceneContext = SceneRenderTargets::Get();

	bool bParallel = false;

	if (!bParallel)
	{
		bDepthWasCleared = PreRenderPrePass(RHICmdList);
		bDidPrePre = true;
	}
	else
	{
		//SceneContext.GetSceneDepthSurface();
	}

	//if (EarlyZPassMode != DDM_None)
	{
		{
			DrawingPolicyRenderState DrawRenderState(View, PassUniformBuffer);
			//if(View.ShouldRenderView())
			{
				DrawRenderState.SetBlendState(TStaicBlendState<0>::GetRHI());
				if (bParallel)
				{

				}
				else
				{
					bDirty = RenderPrePassView(View,DrawRenderState,)
				}
			}
		}
	}

	//if (bDitheredLODTransitionsUseStencil)
	{

	}

	SceneContext.FinishRenderingPrePass(Context);

	return bDepthWasCleared;
}

bool DeferredShadingSceneRenderer::RenderBasePassView(ID3D11DeviceContext* Context, const ViewInfo& InView)
{

}

bool DeferredShadingSceneRenderer::RenderBasePass(ID3D11DeviceContext* Context)
{

}

void DeferredShadingSceneRenderer::Render(ID3D11DeviceContext* Context)
{

}
