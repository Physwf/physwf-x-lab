#pragma once

#include "fbxsdk.h"

class FBXImporter
{
public:
	FBXImporter() {}
	~FBXImporter() {}

	void Initialize();
private:
	FbxManager*			m_fbxManager;
	FbxIOSettings*		m_IOSettings;
	FbxScene*			m_fbxScence;
};