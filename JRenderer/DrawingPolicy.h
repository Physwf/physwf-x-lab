#pragma once

#include "RenderResources.h"

struct DrawingPolicyRenderState
{
	DrawingPolicyRenderState(const FSceneView& SceneView, ID3D11Buffer* InPassUniformBuffer = nullptr) :
		BlendState(nullptr)
		, DepthStencilState(nullptr)
		//, DepthStencilAccess(FExclusiveDepthStencil::DepthRead_StencilRead)
		//, ViewUniformBuffer(SceneView.ViewUniformBuffer)
		, PassUniformBuffer(InPassUniformBuffer)
		//, StencilRef(0)
		//, ViewOverrideFlags(EDrawingPolicyOverrideFlags::None)
		//, DitheredLODTransitionAlpha(0.0f)
	{

	}

	DrawingPolicyRenderState() :
		BlendState(nullptr)
		, DepthStencilState(nullptr)
		, ViewUniformBuffer()
		, PassUniformBuffer(nullptr)
		//, StencilRef(0)
		//, ViewOverrideFlags(EDrawingPolicyOverrideFlags::None)
		//, DitheredLODTransitionAlpha(0.0f)
	{
	}

	void SetBlendState(ID3D11BlendState* InBlendState)
	{
		BlendState = InBlendState;
	}

	const ID3D11BlendState* GetBlendState() const
	{
		return BlendState;
	}

	void SetDepthStencilState(ID3D11DepthStencilState* InDepthStencilState)
	{
		DepthStencilState = InDepthStencilState;
		//StencilRef = 0;
	}

	void SetStencilRef(uint32 InStencilRef)
	{
		//StencilRef = InStencilRef;
	}

	const ID3D11DepthStencilState* GetDepthStencilState() const
	{
		return DepthStencilState;
	}

	// 	void SetDepthStencilAccess(FExclusiveDepthStencil::Type InDepthStencilAccess)
	// 	{
	// 		DepthStencilAccess = InDepthStencilAccess;
	// 	}

	// 	FExclusiveDepthStencil::Type GetDepthStencilAccess() const
	// 	{
	// 		return DepthStencilAccess;
	// 	}

	// 	void SetViewUniformBuffer(const TUniformBufferRef<FViewUniformShaderParameters>& InViewUniformBuffer)
	// 	{
	// 		ViewUniformBuffer = InViewUniformBuffer;
	// 	}

	// 	const TUniformBufferRef<FViewUniformShaderParameters>& GetViewUniformBuffer() const
	// 	{
	// 		return ViewUniformBuffer;
	// 	}

	void SetPassUniformBuffer(ID3D11Buffer* InPassUniformBuffer)
	{
		PassUniformBuffer = InPassUniformBuffer;
	}

	ID3D11Buffer* GetPassUniformBuffer() const
	{
		return PassUniformBuffer;
	}

	// 	uint32 GetStencilRef() const
	// 	{
	// 		return StencilRef;
	// 	}

	// 	void SetDitheredLODTransitionAlpha(float InDitheredLODTransitionAlpha)
	// 	{
	// 		DitheredLODTransitionAlpha = InDitheredLODTransitionAlpha;
	// 	}
	// 
	// 	float GetDitheredLODTransitionAlpha() const
	// 	{
	// 		return DitheredLODTransitionAlpha;
	// 	}
	// 
	// 
	// 	EDrawingPolicyOverrideFlags& ModifyViewOverrideFlags()
	// 	{
	// 		return ViewOverrideFlags;
	// 	}
	// 
	// 	EDrawingPolicyOverrideFlags GetViewOverrideFlags() const
	// 	{
	// 		return ViewOverrideFlags;
	// 	}

	void ApplyToPSO(GraphicsPipelineStateInitializer& GraphicsPSOInit) const
	{
		GraphicsPSOInit.BlendState = BlendState;
		GraphicsPSOInit.DepthStencilState = DepthStencilState;
	}
private:
	ID3D11BlendState * BlendState;
	ID3D11DepthStencilState*	DepthStencilState;
	//FExclusiveDepthStencil::Type	DepthStencilAccess;

	//TUniformBufferRef<FViewUniformShaderParameters>	ViewUniformBuffer;
	ID3D11Buffer*					PassUniformBuffer;
	//uint32							StencilRef;

	//not sure if those should belong here
	//EDrawingPolicyOverrideFlags	ViewOverrideFlags;
	//float							DitheredLODTransitionAlpha;
};

class MeshDrawingPolicy
{
public:
	MeshDrawingPolicy(const VertexFactory* InVF);

	void SetMeshRenderState(ID3D11DeviceContext* Context);
	void DrawMesh(ID3D11DeviceContext* Context);
	void SetupPipelineState();
	void SetShaderState(ID3D11DeviceContext* Context);

	const VertexDeclaration* GetVertexDeclaration() const;
	const VertexFactory* GetVertexFactory() const { return VF; }
protected:
	const VertexFactory* VF;
};
