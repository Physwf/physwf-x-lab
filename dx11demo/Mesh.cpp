#include "Mesh.h"
#include "fbxsdk.h"
#include "log.h"

void Mesh::ImportFromFBX(const char* pFileName)
{
	FbxManager* lFbxManager = FbxManager::Create();

	FbxIOSettings* lIOSetting = FbxIOSettings::Create(lFbxManager, IOSROOT);
	lFbxManager->SetIOSettings(lIOSetting);

	FbxImporter* lImporter = FbxImporter::Create(lFbxManager, "MeshImporter");
	if (!lImporter->Initialize(pFileName,-1, lFbxManager->GetIOSettings()))
	{
		X_LOG("Call to FbxImporter::Initialize() failed.\n");
		X_LOG("Error returned: %s \n\n", lImporter->GetStatus().GetErrorString());
		return;
	}

	FbxScene* lScene = FbxScene::Create(lFbxManager, "Mesh");
	lImporter->Import(lScene);
	lImporter->Destroy();

	FbxNode* lRootNode = lScene->GetRootNode();
	if (lRootNode)
	{
		int SubMeshCount = lRootNode->GetMaterialCount();
		for (int i = 0; i < SubMeshCount; ++i)
		{
			FbxSurfaceMaterial* lMaterial = lRootNode->GetMaterial(i);
		}
	}
}

