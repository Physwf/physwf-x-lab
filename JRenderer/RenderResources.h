#pragma once

#include "CoreMinimal.h"
#include "D3D11API.h"

#define SafeRelease(Resource) if(Resource)Resource->Release()


class RenderResource
{
public:
	virtual ~RenderResource() {}

	virtual void InitRHI() {}
	virtual void ReleaseRHI() {}

	void UpdateRHI();
};

class VertexBuffer : public RenderResource
{
public:
	ID3D11Buffer * Buffer = nullptr;

	virtual void ReleaseRHI() override
	{
		SafeRelease(Buffer);
	}
};

class IndexBuffer : public RenderResource
{
public:
	ID3D11Buffer * Buffer;

	virtual void ReleaseRHI() override
	{
		SafeRelease(Buffer);
	}
};

struct VertexStreamComponent
{
	const VertexBuffer* VB;
	uint32 StreamOffset = 0;
	uint8 Offset = 0;
	uint8 Stride = 0;
	DXGI_FORMAT Type = DXGI_FORMAT_UNKNOWN;
	//EVertexStreamUsage VertexStreamUsage = EVertexStreamUsage::Default;

	VertexStreamComponent(const VertexBuffer* InVB,uint32 InOffset, uint32 InStride, DXGI_FORMAT InType/*, EVertexStreamUsage Usage = EVertexStreamUsage::Default*/)
		:VB(InVB),
		StreamOffset(InOffset),
		Offset(InOffset),
		Stride(InStride),
		Type(InType)
	{}
};



class VertexFactory : public RenderResource
{
public:
	void SetStreams(ID3D11DeviceContext* Context) const;
	void SetPostionStreams(ID3D11DeviceContext* Context) const;


	virtual void ReleaseRHI() override;

	VertexDeclaration* GetDeclaration() { return Delclaration; }
	void SetDeclaration(VertexDeclaration* NewDelclaration) { Delclaration = NewDelclaration; }

	const VertexDeclaration* GetDeclaration() const { return Delclaration; }
	const VertexDeclaration* GetPositionDelcaration() const { return PositionDelclaration; }
protected:
	
	VertexElement AddVertexStream(const VertexStreamComponent& Component, uint8 AttributeIndex);
	VertexElement AddPostionVertexStream(const VertexStreamComponent& Component, uint8 AttributeIndex);

	void InitDeclaration(const VertexElementList_t& Elements);
	void InitPostionDeclaration(const VertexElementList_t& Elements);

	struct VertexStream
	{
		const VertexBuffer* VB = nullptr;
		uint32 Stride = 0;
		uint32 Offset = 0;
		//EVertexStreamUsage VertexStreamUsage = EVertexStreamUsage::Default;

		friend bool operator==(const FVertexStream& A, const FVertexStream& B)
		{
			return A.VB == B.VB 
				&& A.Stride == B.Stride 
				&& A.Offset == B.Offset
				/*&&  A.VertexStreamUsage == B.VertexStreamUsage*/;
		}

		VertexStream()
		{
		}
	};

	std::vector<VertexStream> Streams;
private:
	std::vector<VertexStream> PositionStreams;

	VertexDeclaration * Delclaration;
	VertexDeclaration * PositionDelclaration;
};

class ShaderResource : public RenderResource
{
public:
	ShaderResource();
	ShaderResource(std::vector<uint8>& InCode);

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;
private:
	ID3D11VertexShader*		VS;
	ID3D11PixelShader*		PS;
	ID3D11HullShader*		HS;
	ID3D11DomainShader*		DS;
	ID3D11GeometryShader*	GS;
	ID3D11ComputeShader*	CS;

	std::vector<uint8> Code;
};

class SceneRenderTargets : public RenderResource
{
public:
	virtual ~SceneRenderTargets() {}

	static SceneRenderTargets& Get();

	void BeginRenderingPrePass(ID3D11DeviceContext* Context);
	void FinishRenderingPrePass(ID3D11DeviceContext* Context);


};