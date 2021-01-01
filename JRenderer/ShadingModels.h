#pragma once

#include "d3d12demo.h"
#include "Primitives.h"

using namespace DirectX;

struct ViewUniform
{
	XMFLOAT4X4 WorldToClip;
	XMFLOAT4X4 ClipToWorld;
	XMFLOAT4X4 SvPositionToWorld;
	XMFLOAT4 ViewSizeAndInvSize;
	XMFLOAT4 ViewRectMin;
	float Pading00[8];
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
	virtual void InitPipelineStates();
	virtual void Draw();
private:
	void InitMeshResourcces();
};