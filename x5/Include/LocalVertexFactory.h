#pragma once

#include "ShaderCore/VertexFactory.h"
#include "Components.h"

class FLocalVertexFactory : public FVertexFactory
{
public:
	FLocalVertexFactory(/*ERHIFeatureLevel::Type InFeatureLevel, const char* InDebugName, */const FStaticMeshDataType* InStaticMeshDataType = nullptr)
	{
		StaticMeshDataType = InStaticMeshDataType ? InStaticMeshDataType : &Data;
		bSupportsManualVertexFetch = true;
	}

	struct FDataType : public FStaticMeshDataType
	{

	};

	void SetData(const FDataType& InData);

	void Copy(const FLocalVertexFactory& Other);

	virtual void InitRHI() override;
	virtual void ReleaseRHI()
	{
		FVertexFactory::ReleaseRHI();
	}
protected:
	const FDataType& GetData() const { return Data; }
	FDataType Data;
	const FStaticMeshDataType* StaticMeshDataType;
	int32 ColorStreamIndex;
};

class FLocalVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{

};