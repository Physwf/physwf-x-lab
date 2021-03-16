#pragma once

#include "d3d12demo.h"
#include "Primitives.h"
#include "D3D12RHI.h"

using namespace DirectX;

struct ViewUniform
{
	XMFLOAT4 ViewOrigin;
	XMFLOAT4X4 WorldToClip;
	XMFLOAT4X4 ClipToWorld;
	XMFLOAT4X4 SvPositionToWorld;
	XMFLOAT4 ViewSizeAndInvSize;
	XMFLOAT4 ViewRectMin;
	float Pading00[4];
};

struct PrimitiveUniform
{
	XMFLOAT4X4 LocalToWorld;
	float LocalToWorldDeterminantSign;
	float Pading00[47];
};

struct AmbientLightUniform
{
	XMFLOAT3 AmbientLightColor;
	float Pading00[61];
};

struct PointLightUniform
{
	XMFLOAT4 LightPosition;
	XMFLOAT4 LightColor;
	float Intencity;
	float FadeOffExponent;
	float Pading00[54];
};

struct MaterialUniform
{
	XMFLOAT4 ka;
	XMFLOAT4 kd;
	XMFLOAT4 ks;
	float alpha;
	float ShadingModel;
	float Pading00[50];
};

struct PBRMaterialUniform
{
	XMFLOAT4 BaseColor;
	XMFLOAT4 SpecularColor;
	float fRoughtness;
};

class ShadingModelDemo : public D3D12Demo
{
public:
	ShadingModelDemo(HWND hWnd) : D3D12Demo(hWnd)
	{
		m_NumRTVDescriptors = FrameCount + 1;
		m_NumDSVDescriptors = 1;
	}
protected:
	virtual void InitPipelineStates();
	
	void InitSceneColorRT();
	void InitSceneDepthRT();
protected:
	D3D12_VIEWPORT	m_Viewport;
	D3D12_RECT		m_ScissorRect;
	DXGI_FORMAT		m_RTFormat;
	UINT m_SampleCount;
	UINT m_SampleQuality;
	ComPtr<ID3D12Resource>				m_SceneColorRTMSAA;
	ComPtr<ID3D12Resource>				m_SceneDepthRTMSAA;
	ComPtr<ID3D12RootSignature>			m_PrimitiveRootSign;
	ComPtr<ID3D12PipelineState>			m_PrimitivePipelineState;
	ComPtr<ID3D12GraphicsCommandList>	m_PrimitiveCmdList;
	ComPtr<ID3D12Resource>				m_PrimitiveVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_PrimitiveVertexBufferView;
	ComPtr<ID3D12Resource>				m_PrimitiveIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW				m_PrimitiveIndexBufferView;
	ComPtr<ID3D12Resource>				m_ViewUniformBuffer;
	ComPtr<ID3D12Resource>				m_PrimitiveUniformBuffer;

	Mesh m;
private:
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	m_MSAALevels;
};

class PhongShadingModel : public ShadingModelDemo
{
public:
	PhongShadingModel(HWND hWnd) : ShadingModelDemo(hWnd)
	{
		m_NumCBVSRVUAVDescriptors = 2;
		bUseBlinPhong = true;
	}
	virtual void OnMouseMove(float fScreenX, float fScreenY);
	virtual void OnKeyDown(unsigned char KeyCode);
protected:
	virtual void InitPipelineStates();
	virtual void Draw();
private:
	void InitMeshResourcces();
	void UpdatePointLight(float fScreenX, float fScreenY);
private:
	ComPtr<ID3D12Resource>				m_AmbientLightUniformBuffer;
	ComPtr<ID3D12Resource>				m_PointLightUniformBuffer;
	ComPtr<ID3D12Resource>				m_MaterialUniformBuffer;
	bool bUseBlinPhong;
};

class PBRShadingModel : public ShadingModelDemo
{
public:
	PBRShadingModel(HWND hWnd) : ShadingModelDemo(hWnd)
	{
	}
protected:
	virtual void InitPipelineStates() override;
	void GenEnvironmentMap();
	void DrawSkyBox();
private:
	void LoadGenEnviPipelineState();
	void LoadSkyboxPipelineState();
	void LoadGenEnviAssets();
	void LoadSkyboxAssets();
protected:
	ComPtr<ID3D12Resource>		mEnvironmentMap;
	ComPtr<ID3D12Resource>			mHDRRT;
	D3D12_CPU_DESCRIPTOR_HANDLE		mHDRRTVHandle;
	ComPtr<ID3D12Resource>			mDepthStencial;
	D3D12_CPU_DESCRIPTOR_HANDLE		mDepthStencialHandle;
private:
	ComPtr<ID3D12Resource>		mHDRI;
	ComPtr<ID3D12Resource>		mHDRIUpload;
	D3D12_CPU_DESCRIPTOR_HANDLE mHDRISRVHandle;
	ComPtr<ID3D12RootSignature> mGenEnviMapRootSignature;
	ComPtr<ID3D12PipelineState> mGenEnviMapPSO;
	ComPtr<ID3D12GraphicsCommandList> mGenEnviMapCmdList;
	D3D12_CPU_DESCRIPTOR_HANDLE mGenEnviRTVHandle;
	ComPtr<ID3D12Resource>		mGenEnviMapVB;
	ComPtr<ID3D12Resource>		mGenEnviMapVBUpload;
	D3D12_VERTEX_BUFFER_VIEW	mGenEnviMapVBView;
	ComPtr<ID3D12Resource>		mGenEnviMapConstBuffer;
	D3D12_VIEWPORT				mGenEnviViewport;
	D3D12_RECT					mGenEnviScissorRect;
	//SkyBox
	ComPtr<ID3D12RootSignature> mSkyboxRootSignature;
	ComPtr<ID3D12PipelineState> mSkyboxPSO;
	ComPtr<ID3D12GraphicsCommandList> mSkyboxCommandList;
	ComPtr<ID3D12Resource>		mSkyboxVB;
	D3D12_VERTEX_BUFFER_VIEW	mSkyboxVBView;
	ComPtr<ID3D12Resource>		mSkyboxIB;
	D3D12_INDEX_BUFFER_VIEW		mSkyboxIBView;
	ComPtr<ID3D12Resource>		mSkyboxCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mSkyboxCBVHandle;
};

class PBRShadingModelRealIBL : public PBRShadingModel
{
public:
	PBRShadingModelRealIBL(HWND hWnd) : PBRShadingModel(hWnd)
	{
		m_NumRTVDescriptors = FrameCount + 1;
		m_NumCBVSRVUAVDescriptors = 1;
		m_NumDSVDescriptors = 1;
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
private:
	void LoadPrimitivePipelineState();
	void LoadPrimitiveAssets();
	void UpdatePointLight(float fScreenX, float fScreenY);
	void DrawPrimitives();
	void PostProcess();
private:
	ComPtr<ID3D12RootSignature>		mPrimitiveRootSignature;
	ComPtr<ID3D12PipelineState>		mPrimitvePSO;
	ComPtr<ID3D12GraphicsCommandList> mPrimitiveCommandList;

	Mesh							mPrimitive;
	ComPtr<ID3D12Resource>			mPrimitiveVB;
	D3D12_VERTEX_BUFFER_VIEW		mPrimitiveVBView;
	ComPtr<ID3D12Resource>			mPrimitiveIB;
	D3D12_INDEX_BUFFER_VIEW			mPrimitiveIBView;
	ComPtr<ID3D12Resource>			mPrimitiveViewCB;
	ComPtr<ID3D12Resource>			mPrimitiveMaterialCB;

};

class PBRShadingModelPrefilterIBL : public PBRShadingModel
{
protected:
	void LoadGenIrradiancePipelineState();
	void LoadGenPrefilterEnviPipelineState();
protected:
	ComPtr<ID3D12RootSignature> mGenIrradianceMapRootSignature;
	ComPtr<ID3D12PipelineState> mGenIrradianceMapPSO;
	ComPtr<ID3D12RootSignature> mGenPrefilterEnvironmentMapRootSignature;
	ComPtr<ID3D12PipelineState> mGenPrefilterEnvironmentMapPSO;
};