#pragma once

#include "RenderResources.h"
#include <memory>

class ShaderParameter
{
public:
	ShaderParameter()
		: BufferIndex(0)
		, BaseIndex(0)
		, NumBytes(0)
	{}

	//void Bind(const FShaderParameterMap& ParameterMap, const TCHAR* ParameterName, EShaderParameterFlags Flags = SPF_Optional);
	bool IsBound() const { return NumBytes > 0; }

	uint32 GetBufferIndex() const { return BufferIndex; }
	uint32 GetBaseIndex() const { return BaseIndex; }
	uint32 GetNumBytes() const { return NumBytes; }
private:
	uint16 BufferIndex;
	uint16 BaseIndex;
	// 0 if the parameter wasn't bound
	uint16 NumBytes;
};

class ShaderResourceParameter
{
public:
	ShaderResourceParameter()
		: BaseIndex(0)
		, NumResources(0)
	{}
	
	//void Bind(const FShaderParameterMap& ParameterMap, const TCHAR* ParameterName, EShaderParameterFlags Flags = SPF_Optional);
	bool IsBound() const { return NumResources > 0; }

	uint32 GetBaseIndex() const { return BaseIndex; }
	uint32 GetNumResources() const { return NumResources; }
private:
	uint16 BaseIndex;
	uint16 NumResources;
};

class RWShaderParameter
{
public:
	//void Bind(const FShaderParameterMap& ParameterMap, const TCHAR* BaseName);

	bool IsBound() const
	{
		return SRVParameter.IsBound() || UAVParameter.IsBound();
	}

	bool IsUAVBound() const
	{
		return UAVParameter.IsBound();
	}

	uint32 GetUAVIndex() const
	{
		return UAVParameter.GetBaseIndex();
	}

	template<typename TShaderRHIRef, typename TRHICmdList>
	inline void SetBuffer(TRHICmdList& RHICmdList, TShaderRHIRef Shader, const FRWBuffer& RWBuffer) const;

	template<typename TShaderRHIRef, typename TRHICmdList>
	inline void SetBuffer(TRHICmdList& RHICmdList, TShaderRHIRef Shader, const FRWBufferStructured& RWBuffer) const;

	template<typename TShaderRHIRef, typename TRHICmdList>
	inline void SetTexture(TRHICmdList& RHICmdList, TShaderRHIRef Shader, const FTextureRHIParamRef Texture, FUnorderedAccessViewRHIParamRef UAV) const;

	template<typename TRHICmdList>
	inline void UnsetUAV(TRHICmdList& RHICmdList, FComputeShaderRHIParamRef ComputeShader) const;
private:

	FShaderResourceParameter SRVParameter;
	FShaderResourceParameter UAVParameter;
};

class ShaderUniformBufferParameter
{
public:
	ShaderUniformBufferParameter()
		:BaseIndex(0)
	{}

	uint32 GetBaseIndex() const { return BaseIndex; }
private:
	uint16 BaseIndex;
};

template<typename TBufferStruct>
class TShaderUniformBufferParameter : public ShaderUniformBufferParameter
{
public:

};

template <typename TShaderRHIRef>
inline void SetUniformBufferParameter(TShaderRHIRef ShaderRHI, const ShaderUniformBufferParameter* Parameter, ID3D11Buffer* BufferRHI)
{
	SetShaderUniformBuffer(ShaderRHI, Parameter->GetBaseIndex(), BufferRHI);
}



class Shader
{
public:
	Shader();
	virtual ~Shader();

	void SetResource(ShaderResource* InResource);

	template<typename UniformBufferStructType>
	const TShaderUniformBufferParameter<UniformBufferStructType>& GetUniformBufferParameter() const
	{

	}
private:
	std::shared_ptr<ShaderResource> Resource;
};


class MaterialShader : public Shader
{

};

class MeshMaterialShader : public MaterialShader
{

};

class GlobalShader : public Shader
{
public:
	GlobalShader() : Shader() {}

	template<typename TViewUniformShaderParameters,typename ShaderRHIParamRef>
	inline void SetParameters(ID3D11DeviceContext* Context, ShaderRHIParamRef ShaderRHI, const ID3D11Buffer* ViewUniformBuffer)
	{
		const auto& ViewUniformBufferParameter = static_cast<const ShaderUniformBufferParameter&>(GetUniformBufferParameter());
		SetUniformBufferParameter(ShaderRHI, ViewUniformBufferParameter, ViewUniformBuffer);
	}
};


class BaseHS : public MeshMaterialShader
{

};

class BaseDS : public MeshMaterialShader
{

};
