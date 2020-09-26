#include "RenderResources.h"


void VertexFactory::SetStreams(ID3D11DeviceContext* Context) const
{

}

void VertexFactory::SetPostionStreams(ID3D11DeviceContext* Context) const
{

}

void VertexFactory::ReleaseRHI()
{

}

VertexElement VertexFactory::AddVertexStream(const VertexStreamComponent& Component, uint8 AttributeIndex)
{
	VertexStream Stream;
	Stream.VB = Component.VB;
	Stream.Stride = Component.Stride;
	Stream.Offset = Component.Offset;
	Streams.push_back(Stream);

	return VertexElement(Streams.size() -1,Component.Offset,Component.Type, AttributeIndex,Component.Stride);
}

VertexElement VertexFactory::AddPostionVertexStream(const VertexStreamComponent& Component, uint8 AttributeIndex)
{
	VertexStream Stream;
	Stream.VB = Component.VB;
	Stream.Stride = Component.Stride;
	Stream.Offset = Component.Offset;
	PositionStreams.push_back(Stream);

	return VertexElement(PositionStreams.size() - 1, Component.Offset, Component.Type, AttributeIndex, Component.Stride);
}

void VertexFactory::InitDeclaration(const VertexElementList_t& Elements)
{
	Delclaration = CreateVertexDelcaration(Elements);
}

void VertexFactory::InitPostionDeclaration(const VertexElementList_t& Elements)
{
	PositionDelclaration = CreateVertexDelcaration(Elements);
}

void RenderResource::UpdateRHI()
{
	ReleaseRHI();
	InitRHI();
}

ShaderResource::ShaderResource()
{

}

ShaderResource::ShaderResource(std::vector<uint8>& InCode)
{

}

void ShaderResource::InitRHI()
{

}

void ShaderResource::ReleaseRHI()
{

}

static TGlobalResource<SceneRenderTargets> SceneRenderTargetsSingleton;

SceneRenderTargets& SceneRenderTargets::Get()
{
	return SceneRenderTargetsSingleton;
}

void SceneRenderTargets::BeginRenderingPrePass(ID3D11DeviceContext* Context)
{

}

void SceneRenderTargets::FinishRenderingPrePass(ID3D11DeviceContext* Context)
{

}
