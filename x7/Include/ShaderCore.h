#pragma once

#include "CoreMinimal.h"

struct XShaderTarget
{
	uint32 Frequency : SF_NumBits;
	uint32 Platform : SP_NumBits;

	XShaderTarget()
	{}

	XShaderTarget(EShaderFrequency InFrequency, EShaderPlatform InPlatform)
		:Frequency(InFrequency)
		,Platform(InFrequency)
	{}

	friend bool operator==(const XShaderTarget& X, const XShaderTarget& Y)
	{
		return X.Frequency == Y.Frequency && X.Platform == Y.Platform;
	}
};

class XShaderParameterMap
{
public:
	XShaderParameterMap();

	bool FindParameterAllocation(const TCHAR* ParameterName, uint16& OutBufferIndex, uint16& OutBaseIndex, uint16& OutSize) const;
	bool ContainsParameterAllocation(const TCHAR* ParameterName) const;
	void AddParameterAllocation(const TCHAR* ParameterName, uint16 BufferIndex, uint16 BaseIndex, uint16 Size);
	void RemoveParameterAllocation(const TCHAR* ParameterName);

private:
	struct XParameterAllocation
	{
		uint16 BufferIndex;
		uint16 BaseIndex;
		uint16 Size;
		mutable bool bBound;

		XParameterAllocation() :
			bBound(false)
		{}
	};


};