#include "Mesh.h"
#include "fbxsdk.h"
#include "log.h"
#include "D3D11RHI.h"
#include <d3dcompiler.h>
#include <vector>

void FillFbxArray(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray)
{
	if (pNode->GetMesh())
	{
		pOutMeshArray.push_back(pNode);
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		FillFbxArray(pNode->GetChild(i),pOutMeshArray);
	}
}

FbxAMatrix ComputeTotalMatrix(FbxScene* pScence, FbxNode* pNode, bool bTransformVertexToAbsolute,bool bBakePivotInVertex)
{
	FbxAMatrix Geometry;
	FbxVector4 Translation, Rotation, Scaling;
	Translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	Rotation = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	Scaling = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	FbxAMatrix& GlobalTransform = pScence->GetAnimationEvaluator()->GetNodeGlobalTransform(pNode);
	if (!bTransformVertexToAbsolute)
	{
		if (bBakePivotInVertex)
		{
			FbxAMatrix PivotGeometry;
			FbxVector4 RotationPivot = pNode->GetRotationPivot(FbxNode::eSourcePivot);
			FbxVector4 FullPivot;
			FullPivot[0] = -RotationPivot[0];
			FullPivot[1] = -RotationPivot[1];
			FullPivot[2] = -RotationPivot[2];
			PivotGeometry.SetT(FullPivot);
			Geometry = Geometry * PivotGeometry;
		}
		else
		{
			Geometry.SetIdentity();
		}
	}

	FbxAMatrix TotalMatrix = bTransformVertexToAbsolute ? GlobalTransform * Geometry : Geometry;

	return TotalMatrix;
}

Vector ConvertPos(FbxVector4 pPos)
{
	Vector Result;
	Result.X = (float)pPos[0];
	Result.Y = (float)pPos[1];
	Result.Z = (float)pPos[2];
	return Result;
}

Vector ConvertDir(FbxVector4 Vec)
{
	Vector Result;
	Result.X = (float)Vec[0];
	Result.Y = (float)Vec[1];
	Result.Z = (float)Vec[2];
	return Result;
}

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

	std::vector<FbxNode*> lOutMeshArray;
	FillFbxArray(lScene->GetRootNode(), lOutMeshArray);

	FbxGeometryConverter* GeometryConverter = new FbxGeometryConverter(lFbxManager);

	for (int Index = 0; Index < (int)lOutMeshArray.size(); ++Index)
	{
		FbxNode* lNode = lOutMeshArray[Index];

		if (lNode->GetMesh())
		{
			FbxMesh* lMesh = lNode->GetMesh();

			XStaticMeshSection Section;

			lMesh->RemoveBadPolygons();

			if (!lMesh->IsTriangleMesh())
			{
				FbxNodeAttribute* ConvertedNode = GeometryConverter->Triangulate(lMesh,true);

				if(ConvertedNode!=NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					lMesh = ConvertedNode->GetNode()->GetMesh();
				}
				else
				{
					X_LOG("Cant triangulate!");
				}
			}

			FbxLayer* BaseLayer = lMesh->GetLayer(0);

			bool bSmootingAvaliable = false;
			FbxLayerElementSmoothing* SmoothingInfo = BaseLayer->GetSmoothing();
			FbxLayerElement::EReferenceMode SmoothingReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode SmoothingMappingMode(FbxLayerElement::eByEdge);
			if (SmoothingInfo)
			{
				if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByPolygon)
				{
					GeometryConverter->ComputeEdgeSmoothingFromPolygonSmoothing(lMesh, 0);
				}

				if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByEdge)
				{
					bSmootingAvaliable = true;
				}

				SmoothingReferenceMode = SmoothingInfo->GetReferenceMode();
				SmoothingMappingMode = SmoothingInfo->GetMappingMode();
			}

			FbxLayerElementVertexColor* LayerElementVertexColor = BaseLayer->GetVertexColors();
			FbxLayerElement::EReferenceMode VertexColorReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode VertexColorMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementVertexColor)
			{
				VertexColorReferenceMode = LayerElementVertexColor->GetReferenceMode();
				VertexColorMappingMode = LayerElementVertexColor->GetMappingMode();
			}

			FbxLayerElementNormal* LayerElementNormal = BaseLayer->GetNormals();
			FbxLayerElementTangent* LayerElementTangent = BaseLayer->GetTangents();
			FbxLayerElementBinormal* LayerElementBinormal = BaseLayer->GetBinormals();

			bool bHasNTBInfomation = LayerElementNormal && LayerElementTangent && LayerElementBinormal;

			FbxLayerElement::EReferenceMode NormalReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode NormalMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementNormal)
			{
				NormalReferenceMode = LayerElementNormal->GetReferenceMode();
				NormalMappingMode = LayerElementNormal->GetMappingMode();
			}

			FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementTangent)
			{
				TangentReferenceMode = LayerElementTangent->GetReferenceMode();
				TangentMappingMode = LayerElementTangent->GetMappingMode();
			}

			FbxLayerElement::EReferenceMode BinormalReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode BinormalMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementBinormal)
			{
				BinormalReferenceMode = LayerElementBinormal->GetReferenceMode();
				BinormalMappingMode = LayerElementBinormal->GetMappingMode();
			}

			FbxAMatrix TotalMatrix;
			FbxAMatrix TotalMatrixForNormal;
			TotalMatrix = ComputeTotalMatrix(lScene, lNode, true, false);
			TotalMatrixForNormal = TotalMatrix.Inverse();
			TotalMatrixForNormal = TotalMatrixForNormal.Transpose();


			int VertexCount = lMesh->GetControlPointsCount();
			auto VertexOffset = mVertices.size();
			auto VertexInstanceOffset = mIndices.size();

			Section.FirstIndex = VertexInstanceOffset;
			Section.MinVertexIndex = VertexInstanceOffset;
			Section.MaxVertexIndex = VertexInstanceOffset + VertexCount;

			for (auto VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
			{
				int RealVertexIndex = VertexOffset + VertexIndex;
				FbxVector4 FbxPosition = lMesh->GetControlPoints()[VertexIndex];
				FbxPosition = TotalMatrix.MultT(FbxPosition);
				Vector const VectorPositon = ConvertPos(FbxPosition);
				int VertexID = CreateVertex();
				mVertices[VertexID].Postion = VectorPositon;
			}

			int PolygonCount = lMesh->GetPolygonCount();
			int CurrentVertexInstanceIndex = 0;
			int SkippedVertexInstance = 0;

			Section.NumTriangles = lMesh->GetPolygonCount();
			Sections.push_back(Section);

			for (auto PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
			{
				int PolygonVertexCount = lMesh->GetPolygonSize(PolygonIndex);
				for (auto CornerIndex = 0; CornerIndex < PolygonVertexCount; ++CornerIndex, CurrentVertexInstanceIndex++)
				{
					//int VertexInstanceIndex = vertex
					const int iControlPointIndex = lMesh->GetPolygonVertex(PolygonIndex, CornerIndex);
					mIndices.push_back(iControlPointIndex + VertexOffset);
					int RealFbxVertexIndex = SkippedVertexInstance + CurrentVertexInstanceIndex;

					if (LayerElementVertexColor)
					{
						int VertexColorMappingIndex = (VertexColorMappingMode == FbxLayerElement::eByControlPoint) ? lMesh->GetPolygonVertex(PolygonIndex, CornerIndex) : RealFbxVertexIndex;
						int VertexColorIndex = (VertexColorReferenceMode == FbxLayerElement::eDirect) ? VertexColorMappingIndex : LayerElementVertexColor->GetIndexArray().GetAt(VertexColorMappingIndex);
						FbxColor VertexColor = LayerElementVertexColor->GetDirectArray().GetAt(VertexColorIndex);
						//todo setcolor
					}

					if (LayerElementNormal)
					{
						int NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ? iControlPointIndex : RealFbxVertexIndex;
						int NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ? NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);
						FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
						TempValue = TotalMatrixForNormal.MultT(TempValue);
						Vector TangentZ = ConvertDir(TempValue);
						mVertices;
					}
				}
			}
		}
	}

	delete GeometryConverter;
	GeometryConverter = nullptr;
}

void Mesh::InitResource()
{
	HRESULT hr;

	D3D11_BUFFER_DESC VertexDesc;
	ZeroMemory(&VertexDesc, sizeof(VertexDesc));
	VertexDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexDesc.ByteWidth = sizeof(Vertex) * mVertices.size();
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexDesc.CPUAccessFlags = 0;
	VertexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mVertices[0];
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	hr = D3D11Device->CreateBuffer(&VertexDesc, &InitData, &VertexBuffer);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateBuffer failed!");
		return;
	}

	D3D11_BUFFER_DESC IndexDesc;
	ZeroMemory(&IndexDesc, sizeof(IndexDesc));
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexDesc.ByteWidth = sizeof(unsigned int) * mIndices.size();
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexDesc.CPUAccessFlags = 0;
	IndexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexData;
	ZeroMemory(&IndexData, sizeof(IndexData));
	IndexData.pSysMem = &mIndices[0];
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	hr = D3D11Device->CreateBuffer(&IndexDesc, &IndexData, &IndexBuffer);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateBuffer failed!");
		return;
	}

	D3D11_BUFFER_DESC ConstBufferDesc;
	ZeroMemory(&ConstBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ConstBufferDesc.ByteWidth = sizeof(XMMATRIX);
	ConstBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA ConstInitData;
	XMMATRIX World = XMMatrixIdentity();
	ConstInitData.pSysMem = &World;
	ConstInitData.SysMemPitch = 0;
	ConstInitData.SysMemSlicePitch = 0;

	hr = D3D11Device->CreateBuffer(&ConstBufferDesc, &ConstInitData, &ConstantBuffer);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateBuffer failed!");
		return;
	}

	ID3DBlob* VSByteCode;
	ID3DBlob* PSByteCode;
	ID3DBlob* ErrorMsg;
	LPCSTR VSTarget = D3D11Device->GetFeatureLevel() > D3D_FEATURE_LEVEL_11_0 ? "vs_5_0" : "vs_4_0";
	UINT VSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(TEXT("Mesh.hlsl"),NULL,NULL,"VS_Main", VSTarget, VSFlags, 0, &VSByteCode, &ErrorMsg);
	if (FAILED(hr))
	{
		X_LOG("D3DCompileFromFile failed! %s", (const char*)ErrorMsg->GetBufferPointer());
		return;
	}

	LPCSTR PSTarget = D3D11Device->GetFeatureLevel() > D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0";
	UINT PSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
	hr = D3DCompileFromFile(TEXT("Mesh.hlsl"), NULL, NULL, "PS_Main", PSTarget, PSFlags, 0, &PSByteCode, &ErrorMsg);
	if (FAILED(hr))
	{
		X_LOG("D3DCompileFromFile failed! %s", (const char*)ErrorMsg->GetBufferPointer());
		return;
	}

	hr = D3D11Device->CreateVertexShader(VSByteCode->GetBufferPointer(), VSByteCode->GetBufferSize(), NULL, &VertexShader);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateVertexShader failed!");
		return;
	}
	hr = D3D11Device->CreatePixelShader(PSByteCode->GetBufferPointer(), PSByteCode->GetBufferSize(), NULL, &PixelShader);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreatePixelShader failed!");
		return;
	}



	D3D11_INPUT_ELEMENT_DESC InputDesc[] = 
	{
		{"POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0, D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"TEXCOORD",	1,	DXGI_FORMAT_R32G32_FLOAT,		0, 32, D3D11_INPUT_PER_VERTEX_DATA,	0},
	};

	UINT NumElement = ARRAYSIZE(InputDesc);

	hr = D3D11Device->CreateInputLayout(InputDesc, NumElement, VSByteCode->GetBufferPointer(), VSByteCode->GetBufferSize(), &InputLayout);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateInputLayout failed!");
		return;
	}

	if (VSByteCode)
	{
		VSByteCode->Release();
	}
	if (PSByteCode)
	{
		PSByteCode->Release();
	}
	if (ErrorMsg)
	{
		ErrorMsg->Release();
	}
}

void Mesh::ReleaseResource()
{
	if (VertexBuffer)
	{
		VertexBuffer->Release();
	}
	if (VertexShader)
	{
		VertexShader->Release();
	}
	if (PixelShader)
	{
		PixelShader->Release();
	}
	if (InputLayout)
	{
		InputLayout->Release();
	}
}

void Mesh::Draw()
{
	D3D11DeviceContext->IASetInputLayout(InputLayout);
	D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT Stride = sizeof(Vertex);
	UINT Offset = 0;

	D3D11DeviceContext->VSSetConstantBuffers(1, 1, &ConstantBuffer);
	Yall(0.01f);
	XMMATRIX World = GetWorldMatrix();
	D3D11DeviceContext->UpdateSubresource(ConstantBuffer, 0, 0, &World, 0, 0);

	D3D11DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	D3D11DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D11DeviceContext->VSSetShader(VertexShader, 0, 0);
	D3D11DeviceContext->PSSetShader(PixelShader, 0, 0);
	for (const auto& Section : Sections)
	{
		D3D11DeviceContext->DrawIndexed(Section.NumTriangles * 3, Section.FirstIndex, 0);
	}
}

int Mesh::CreateVertex()
{
	mVertices.push_back(Vertex());
	return (int)(mVertices.size() - 1);
}

