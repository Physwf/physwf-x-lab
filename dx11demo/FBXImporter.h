#pragma once

#include "fbxsdk.h"

class FBXWalker
{
public:
	FBXWalker() {}
	~FBXWalker() {}

	void Initialize();
private:
	FbxManager*			m_fbxManager;
	FbxIOSettings*		m_IOSettings;
	FbxScene*			m_fbxScence;
};