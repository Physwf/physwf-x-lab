#pragma once

#include "d3d12demo.h"
#include "DirectXMath.h"
using namespace DirectX;

class ShadowDemo : public D3D12Demo
{
public:
	ShadowDemo(HWND hWnd) : D3D12Demo(hWnd)
	{
		m_NumCBVSRVUAVDescriptors += 1;
		m_NumDSVDescriptors += 1;
	}
protected:
	void LoadCommonAssets();
protected:

	ComPtr<ID3D12Resource>		mMarryVBUpload;
	ComPtr<ID3D12Resource>		mMarryVB;
	D3D12_VERTEX_BUFFER_VIEW	mMarryVBView;
	ComPtr<ID3D12Resource>		mMarryIBUpload;
	ComPtr<ID3D12Resource>		mMarryIB;
	D3D12_INDEX_BUFFER_VIEW		mMarryIBView;
	ComPtr<ID3D12Resource>		mMarryPrimitiveCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mMarryPrimitiveCBView;
	ComPtr<ID3D12Resource>		mSceneDepth;
	D3D12_CPU_DESCRIPTOR_HANDLE mSceneDepthViewHandle;
};

class PCSSDemo : public ShadowDemo
{
public:
	PCSSDemo(HWND hWnd) : ShadowDemo(hWnd)
	{
		m_NumCBVSRVUAVDescriptors += 2;
		m_NumDSVDescriptors += 1;
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
private:
	void DrawShadow();
private:
	void LoadPCSSPipleState();
	void LoadScenePipelineState();

	void LoadPCSSAssets();
	void LoadSceneAssets();
private:
	//Shadow Pass
	ComPtr<ID3D12RootSignature> mPCSSRootSignature;
	ComPtr<ID3D12PipelineState> mPCSSPSO;
	ComPtr<ID3D12GraphicsCommandList> mPCSSCommandList;
	ComPtr<ID3D12Resource>		mPCSSDetph;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSDetphViewHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSDetphSRVHandle;
	ComPtr<ID3D12Resource>		mPCSSViewCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mPCSSViewCBViewHandle;
	//Scene
	ComPtr<ID3D12Resource>		mSceneViewCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mSceneViewCBHandle;
	//Marry pass
	ComPtr<ID3D12RootSignature> mMarrayRootSignature;
	ComPtr<ID3D12PipelineState> mMarrayPSO;
	ComPtr<ID3D12GraphicsCommandList> mMarrayCommandList;
	ComPtr<ID3D12Resource>		mMarryMaterialCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mMarryMaterialCBView;
	ComPtr<ID3D12Resource>		mMarrayDiffuseColorSR;
	D3D12_CPU_DESCRIPTOR_HANDLE mMarrayDiffuseColorSRV;
};