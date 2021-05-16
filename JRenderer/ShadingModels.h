#pragma once

#include "d3d12demo.h"
#include "Primitives.h"
#include "D3D12RHI.h"

using namespace DirectX;

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
	char padding[256 - 2 * sizeof(XMFLOAT4) - sizeof(float)];
};

struct TonemapUniform
{
	int InputWidth;
	int InputHeight;
	float MinLogL;
	float LogRange;
	float InverseLogRange;
	float DeltaTime;
	float tau;
	char padding[256 - 7 * sizeof(float)];
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
		//EnviMapPass, 1 Cube Map
		//General HDR Render Target, 1
		m_NumRTVDescriptors += 2; 
		m_NumDSVDescriptors += 1;
		//EnviMapPass, 2 
		//SkyboxPass, 2
		//Scene SRV 1
		//tonemap 1 CBV 2 SRV 2 UAV
		m_NumCBVSRVUAVDescriptors += 10;
	}
protected:
	virtual void InitPipelineStates() override;
	void GenEnvironmentMap();
	void DrawSkyBox();
	void PostProcess();
private:
	void LoadGenEnviPipelineState();
	void LoadSkyboxPipelineState();
	void LoadTonemapPipelineState();
	void LoadGenEnviAssets();
	void LoadSkyboxAssets();
	void LoadCommonAssets();
	void LoadTonemapAssets();
protected:
	ComPtr<ID3D12Resource>		mEnvironmentMap;
	D3D12_CPU_DESCRIPTOR_HANDLE mEnviromentMapSRV;
	ComPtr<ID3D12Resource>			mHDRRT;
	D3D12_CPU_DESCRIPTOR_HANDLE		mHDRSRV;
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
	D3D12_CPU_DESCRIPTOR_HANDLE mGenEnviMapCBView;
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
	//Tonemap
	ComPtr<ID3D12RootSignature> mTonemapHistogramRootSignature;
	ComPtr<ID3D12PipelineState> mTonemapHistogramPSO;
	ComPtr<ID3D12GraphicsCommandList> mTonemapHistgramCommandList;
	ComPtr<ID3D12Resource>	mTonemapHistogramCB;
	D3D12_CPU_DESCRIPTOR_HANDLE	mTonemapHistogramCBVHandle;
	ComPtr<ID3D12Resource>  mTonemapHistogramResult;
	D3D12_CPU_DESCRIPTOR_HANDLE mTonemapHistogramResultUAVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE mTonemapHistogramResultSRVHandle;

	ComPtr<ID3D12RootSignature> mTonemapAvgLuminanceRootSignature;
	ComPtr<ID3D12PipelineState> mTonemapAvgLuminacePSO;
	ComPtr<ID3D12GraphicsCommandList> mTonemapAvgLuminaceCommandList;
	ComPtr<ID3D12Resource> mTonemapAvgLuminaceBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE mTonemapAvgLuminaceBufferUAVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE mTonemapAvgLuminaceBufferSRVHandle;

	ComPtr<ID3D12RootSignature> mTonemapRootSignature;
	ComPtr<ID3D12PipelineState> mTonemapPSO;
	ComPtr<ID3D12GraphicsCommandList> mTonemapCommandList;
	ComPtr<ID3D12Resource>		mTonemapVBUpload;
	ComPtr<ID3D12Resource>		mTonemapVB;
	D3D12_VERTEX_BUFFER_VIEW	mTonemapVBView;
	ComPtr<ID3D12Resource>		mTonemapCB;
	D3D12_CPU_DESCRIPTOR_HANDLE mTonemapCBVHandle;
};

class PBRShadingModelRealIBL : public PBRShadingModel
{
public:
	PBRShadingModelRealIBL(HWND hWnd) : PBRShadingModel(hWnd)
	{
		//Primitive Pass 3
		m_NumCBVSRVUAVDescriptors += 3;
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;

private:
	void LoadPrimitivePipelineState();
	void LoadPrimitiveAssets();
	void UpdatePointLight(float fScreenX, float fScreenY);
	void DrawPrimitives();
private:
	ComPtr<ID3D12RootSignature>		mPrimitiveRootSignature;
	ComPtr<ID3D12PipelineState>		mPrimitvePSO;
	ComPtr<ID3D12GraphicsCommandList> mPrimitiveCommandList;

	Mesh							mPrimitive;
	ComPtr<ID3D12Resource>			mPrimitiveVB;
	D3D12_VERTEX_BUFFER_VIEW		mPrimitiveVBView;
	ComPtr<ID3D12Resource>			mPrimitiveIB;
	D3D12_INDEX_BUFFER_VIEW			mPrimitiveIBView;
	ComPtr<ID3D12Resource>			mPrimitiveCB;
	D3D12_CPU_DESCRIPTOR_HANDLE		mPrimitiveCBV;
	ComPtr<ID3D12Resource>			mPrimitiveViewCB;
	D3D12_CPU_DESCRIPTOR_HANDLE		mPrimitiveViewCBV;
	ComPtr<ID3D12Resource>			mPrimitiveMaterialCB;
	D3D12_CPU_DESCRIPTOR_HANDLE		mPrimitiveMaterialCBV;

};

class PBRShadingModelPrecomputeIBL : public PBRShadingModel
{
public:
	PBRShadingModelPrecomputeIBL(HWND hWnd) : PBRShadingModel(hWnd)
	{
		m_NumCBVSRVUAVDescriptors += 3;//

		m_NumRTVDescriptors += 11;
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
	virtual void OnMouseMove(float fScreenX, float fScreenY);

	void GenPrefilterEnvironmentMap();
	void GenIntegratedBRDF();
	void DrawPrimitives();
private:
	void LoadGenPrefilterEnviPipelineState();
	void LoadGenIntegratedBRDFPipelineState();
	void LoadPrimitiveBRDFPipelineState();
	void LoadGenPrefilterEnvironmentAssets();
	void LoadGenIntegratedBRDFAssets();
	void LoadPrimitiveAssets();
private:
	ComPtr<ID3D12RootSignature> mGenPrefilterEnvironmentMapRootSignature;
	ComPtr<ID3D12PipelineState> mGenPrefilterEnvironmentMapPSO;
	ComPtr<ID3D12GraphicsCommandList>	mGenPrefilterEnvironmentCmdList;
	ComPtr<ID3D12Resource>		mPrefilterEnvironmentMap;
	ComPtr<ID3D12Resource>		mGenPrefilterEnviromentVB;
	ComPtr<ID3D12Resource>		mGenPrefilterEnviromentVBUpload;
	D3D12_VERTEX_BUFFER_VIEW	mGenPrefilterEnviromentVBView;
	ComPtr<ID3D12Resource>		mGenPrefilterEnviromentIB;
	ComPtr<ID3D12Resource>		mGenPrefilterEnviromentIBUpload;
	D3D12_INDEX_BUFFER_VIEW		mGenPrefilterEnvironmentIBView;
	ComPtr<ID3D12Resource>		mGenPrefilterEnvironmentCBView;
	ComPtr<ID3D12DescriptorHeap> mGenPrefilterEnvironmentDH;

	ComPtr<ID3D12RootSignature> mGenIntegratedBRDFRootSignature;
	ComPtr<ID3D12PipelineState> mGenIntegratedBRDFPSO;
	ComPtr<ID3D12GraphicsCommandList>	mGenIntegratedBRDFCmdList;
	ComPtr<ID3D12Resource>		mIntegratedBRDF;
	ComPtr<ID3D12Resource>		mGenIntegratedBRDFVB;
	ComPtr<ID3D12Resource>		mGenIntegratedBRDFVBUpload;
	D3D12_VERTEX_BUFFER_VIEW	mGenIntegratedBRDFVBView;
	ComPtr<ID3D12DescriptorHeap> mGenIntegratedBRDFDH;

	ComPtr<ID3D12RootSignature> mPrimitiveRootSignature;
	ComPtr<ID3D12PipelineState> mPrimitivePSO;
	ComPtr<ID3D12GraphicsCommandList> mPrimitiveCommandList;
	ComPtr<ID3D12DescriptorHeap> mPrimitiveDH;

	Mesh							mPrimitive;
	ComPtr<ID3D12Resource>			mPrimitiveVB;
	ComPtr<ID3D12Resource>			mPrimitiveVBUpload;
	D3D12_VERTEX_BUFFER_VIEW		mPrimitiveVBView;
	ComPtr<ID3D12Resource>			mPrimitiveIB;
	ComPtr<ID3D12Resource>			mPrimitiveIBUpload;
	D3D12_INDEX_BUFFER_VIEW			mPrimitiveIBView;
	ComPtr<ID3D12Resource>			mPrimitiveCB;
	D3D12_CPU_DESCRIPTOR_HANDLE		mPrimitiveCBV;
	ComPtr<ID3D12Resource>			mPrimitiveViewCB;
	D3D12_CPU_DESCRIPTOR_HANDLE		mPrimitiveViewCBV;
	ComPtr<ID3D12Resource>			mPrimitiveMaterialCB;
	D3D12_CPU_DESCRIPTOR_HANDLE		mPrimitiveMaterialCBV;
};