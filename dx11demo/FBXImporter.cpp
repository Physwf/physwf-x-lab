#include "FBXImporter.h"

void FBXWalker::Initialize()
{
	m_fbxManager = FbxManager::Create();

	m_IOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);

	m_fbxManager->SetIOSettings(m_IOSettings);

	FbxImporter* lImporter = FbxImporter::Create(m_fbxManager, "");

	const char* lFileName = "file.fbx";

	if (!lImporter->Initialize(lFileName, -1, m_fbxManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s \n\n",lImporter->GetStatus().GetErrorString());
		return;
	}

	FbxScene* lScene = FbxScene::Create(m_fbxManager, "myScene");
	lImporter->Import(lScene);
	lImporter->Destroy();

	FbxNode* lRootNode = lScene->GetRootNode();
	if (lRootNode)
	{
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
		{

		}
	}
}

