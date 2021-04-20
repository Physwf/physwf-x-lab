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
	ComPtr<ID3D12DescriptorHeap> mMarryShadowPassDH;
	ComPtr<ID3D12Resource>		mFloorVBUpload;
	ComPtr<ID3D12Resource>		mFloorVB;
	D3D12_VERTEX_BUFFER_VIEW	mFloorVBView;
	ComPtr<ID3D12Resource>		mFloorIBUpload;
	ComPtr<ID3D12Resource>		mFloorIB;
	D3D12_INDEX_BUFFER_VIEW		mFloorIBView;
	ComPtr<ID3D12Resource>		mFloorPrimitiveCB;
	ComPtr<ID3D12DescriptorHeap> mFloorShadowPassDH;

	ComPtr<ID3D12Resource>		mSceneDepth;
	D3D12_CPU_DESCRIPTOR_HANDLE mSceneDepthViewHandle;

	ComPtr<ID3D12Resource>		mPCSSDetph;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSDetphViewHandle;
	ComPtr<ID3D12Resource>		mPCSSViewCB;
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
	void LoadFloorPipelineState();

	void LoadMarryAssets();
	void LoadFloorAssets();
	void LoadCommonAssets();

private:

	//Shadow Pass
	ComPtr<ID3D12RootSignature> mPCSSRootSignature;
	ComPtr<ID3D12PipelineState> mPCSSPSO;
	ComPtr<ID3D12GraphicsCommandList> mPCSSCommandList;
	//Common
	ComPtr<ID3D12Resource>		mSceneViewCB;
	ComPtr<ID3D12Resource>		mLightCB;
	//scene pass Marry
	ComPtr<ID3D12RootSignature> mMarryRootSignature;
	ComPtr<ID3D12PipelineState> mMarryPSO;
	ComPtr<ID3D12GraphicsCommandList> mMarryCommandList;
	ComPtr<ID3D12Resource>		mMarryMaterialCB;
	ComPtr<ID3D12Resource>		mMarrayDiffuseColorSR;
	ComPtr<ID3D12Resource>		mMarrayDiffuseColorSRUpload;
	ComPtr<ID3D12DescriptorHeap> mMarryScenePassDH;
	//scene pass Floor
	ComPtr<ID3D12RootSignature> mFloorRootSignature;
	ComPtr<ID3D12PipelineState> mFloorPSO;
	ComPtr<ID3D12GraphicsCommandList> mFloorCommandList;
	ComPtr<ID3D12Resource>		mFloorMaterialCB;
	ComPtr<ID3D12DescriptorHeap> mFloorScenePassDH;
};