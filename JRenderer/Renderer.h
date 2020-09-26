#pragma once

#include "SceneView.h"
#include "Shader.h"

template <bool bUsePositionOnlyStream>
class TDepthOnlyVS : public MeshMaterialShader
{
	TDepthOnlyVS() {}
};

class DepthOnlyHS : public BaseHS
{

};

class DepthOnlyDS : public BaseDS
{

};

class DepthOnlyPS : public MeshMaterialShader
{

};

class DepthDrawingPolicy : public MeshDrawingPolicy
{
public:
	DepthDrawingPolicy(const FVertexFactory* InVF);

	void SetSharedState(ID3D11DeviceContext* Context);
private:
	class DepthOnlyHS* HullShader;
	class DepthOnlyDS* HullShader;

	TDepthOnlyVS<false>* VertexShader;
	DepthOnlyPS* PixelShader;
};

class PositionOnlyDepthDrawingPolicy : public FMeshDrawingPolicy
{
public:
private:
	TDepthOnlyVS<true> * VertexShader;
};

class DepthDrawingPolicyFactory
{
public:
	static void AddStaticMesh();
	static bool DrawDynamicMesh();
	static bool DrawStaticMesh();
private:
	static bool DrawMesh();
};


class ViewInfo : public SceneView
{

};

class SceneRenderer
{
public:
	class Scene* scene;

	ViewInfo View;

public:
	SceneRenderer(const SceneView* View);
	virtual ~SceneRenderer();

	virtual void Render(ID3D11DeviceContext* Context) = 0;

	static SceneRenderer* CreateSceneRenderer();
};

class DeferredShadingSceneRenderer : public SceneRenderer
{
public:

	//EDepthDrawingMode EarlyZPassMode;
	bool bDitheredLODTransitionsUseStencil;

	DeferredShadingSceneRenderer(const SceneView* View);

	bool InitViews(ID3D11DeviceContext* Context);

	bool PreRenderPrePass(ID3D11DeviceContext* Context);
	bool RenderPrePassView(ID3D11DeviceContext* Context, const ViewInfo& InView);
	bool RenderPrePass(ID3D11DeviceContext* Context);
	bool RenderBasePassView(ID3D11DeviceContext* Context,const ViewInfo& InView);
	bool RenderBasePass(ID3D11DeviceContext* Context);

	virtual void Render(ID3D11DeviceContext* Context) override;
};