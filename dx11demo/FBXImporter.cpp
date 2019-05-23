#include "FBXImporter.h"

void FBXImporter::Initialize()
{
	m_fbxManager = FbxManager::Create();

	m_IOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
}

