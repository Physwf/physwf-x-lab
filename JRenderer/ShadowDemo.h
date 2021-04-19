#pragma once

#include "d3d12demo.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "Primitives.h"

struct ObjMaterialUniform
{
	XMFLOAT3 ka;
	XMFLOAT3 kd;
	XMFLOAT3 ks;
	float Ns;//高光指数
	float Ni;//折射率
	float alpha;
	float ShadingModel;
	float Pading00[256-sizeof(XMFLOAT3)*3-sizeof(float)*4];
};

struct LightUniform
{
	XMFLOAT4 LightPositionAndRadius;
	XMFLOAT4 LightOrientationAndNeerPlane;
	XMFLOAT2 LightmapViewport;
	XMFLOAT3 AmbientIntencity;
	XMFLOAT3 Intencity;
};

class ShadowDemo : public D3D12Demo
{
public:
	ShadowDemo(HWND hWnd) : D3D12Demo(hWnd)
	{
		m_NumCBVSRVUAVDescriptors += 2;//Primitive uniform x 2
		m_NumDSVDescriptors += 2;
	}
protected:
	void LoadCommonAssets();
protected:
	//Mesh
	Mesh mMarry;
	Mesh mPlane;

	ComPtr<ID3D12Resource>		mMarryVBUpload;
	ComPtr<ID3D12Resource>		mMarryVB;
	D3D12_VERTEX_BUFFER_VIEW	mMarryVBView;
	ComPtr<ID3D12Resource>		mMarryIBUpload;
	ComPtr<ID3D12Resource>		mMarryIB;
	D3D12_INDEX_BUFFER_VIEW		mMarryIBView;
	ComPtr<ID3D12Resource>		mMarryPrimitiveCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mMarryPrimitiveCBView;
	ComPtr<ID3D12Resource>		mPlaneVBUpload;
	ComPtr<ID3D12Resource>		mPlaneVB;
	D3D12_VERTEX_BUFFER_VIEW	mPlaneVBView;
	ComPtr<ID3D12Resource>		mPlaneIBUpload;
	ComPtr<ID3D12Resource>		mPlaneIB;
	D3D12_INDEX_BUFFER_VIEW		mPlaneIBView;
	ComPtr<ID3D12Resource>		mPlanePrimitiveCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mPlanePrimitiveCBView;
	ComPtr<ID3D12Resource>		mSceneDepth;
	D3D12_CPU_DESCRIPTOR_HANDLE mSceneDepthViewHandle;

	ComPtr<ID3D12Resource>		mPCSSDetph;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSDetphViewHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSDetphSRVHandle;
	ComPtr<ID3D12Resource>		mPCSSViewCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSViewCBViewHandle;
};

class PCSSDemo : public ShadowDemo
{
public:
	PCSSDemo(HWND hWnd) : ShadowDemo(hWnd)
	{
		m_NumCBVSRVUAVDescriptors += 7;
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
private:
	void DrawShadow();
private:
	void LoadPCSSPipleState();
	void LoadMarryPipelineState();
	void LoadPlanePipelineState();

	void LoadMarryAssets();
	void LoadPlaneAssets();
	void LoadCommonAssets();

private:

	//Shadow Pass
	ComPtr<ID3D12RootSignature> mPCSSRootSignature;
	ComPtr<ID3D12PipelineState> mPCSSPSO;
	ComPtr<ID3D12GraphicsCommandList> mPCSSCommandList;
	//Common
	ComPtr<ID3D12Resource>		mSceneViewCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mSceneViewCBHandle;
	ComPtr<ID3D12Resource>		mLightCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mLightCBView;
	//Marry pass
	ComPtr<ID3D12RootSignature> mMarryRootSignature;
	ComPtr<ID3D12PipelineState> mMarryPSO;
	ComPtr<ID3D12GraphicsCommandList> mMarryCommandList;
	ComPtr<ID3D12Resource>		mMarryMaterialCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mMarryMaterialCBView;
	ComPtr<ID3D12Resource>		mMarrayDiffuseColorSR;
	ComPtr<ID3D12Resource>		mMarrayDiffuseColorSRUpload;
	D3D12_CPU_DESCRIPTOR_HANDLE mMarrayDiffuseColorSRV;
	//Plane
	ComPtr<ID3D12RootSignature> mPlaneRootSignature;
	ComPtr<ID3D12PipelineState> mPlanePSO;
	ComPtr<ID3D12GraphicsCommandList> mPlaneCommandList;
	ComPtr<ID3D12Resource>		mPlaneMaterialCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mPlaneMaterialCBView;
};