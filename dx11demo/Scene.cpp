#include "Scene.h"
#include "log.h"
#include "D3D11RHI.h"

void Scene::InitResource()
{
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth = sizeof(LIGHT_CBUFFER);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &LightConstBuffer;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = D3D11Device->CreateBuffer(&Desc, &InitData, &ConstantBuffer);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateBuffer failed!");
		return;
	}
}

void Scene::ReleaseResource()
{
	if (ConstantBuffer)
	{
		ConstantBuffer->Release();
	}

	for (Camera* C : Cameras)
	{
		C->ReleaseResource();
	}

	for (Mesh* M : Meshes)
	{
		M->ReleaseResource();
	}
}

void Scene::Setup()
{
	Mesh* M = new Mesh();
	//M->ImportFromFBX("./4gikevqde8-IronMan/ironman.fbx");
	//M->ImportFromFBX("./Free_House_FBX/Free House FBX.FBX");
	M->ImportFromFBX("./k526efluton4-House_15/247_House 15_fbx.FBX");
	M->SetPosition(0.0f, 10.0f, 500.0f);
	M->SetRotation(-3.14f / 2.0f, 0.0f, 0.0f);
	M->InitResource();
	Meshes.push_back(M);

	Mesh* Plane = new Mesh();
	Plane->GeneratePlane(2000.0f, 2000.0f, 200, 200);
	Plane->SetPosition(0.0f, 0.0f, 500.0f);
	Plane->SetRotation(3.14f / 2.0f, 0.0f, 0.0f);
	Plane->InitResource();
	Meshes.push_back(Plane);


	Camera* C = new Camera();
	C->SetPostion({ 0, 200, -100 });
	C->LookAt({ 0, 200, 0 });
	C->SetViewport(720, 720);
	C->SetLen(10, 1000);
	C->InitResource();
	CurrentCamera = C;

	Cameras.push_back(C);

	LightConstBuffer.AmLight.Color = { 0.8f,0.8f,0.8f };

	LightConstBuffer.DirLight.Direction = { 1,1,1 };
	LightConstBuffer.DirLight.Color = { 0.8f,0.7f,0.9f };

	LightConstBuffer.PointLights[0].PoistionAndRadias = { 500.f,100.f, 200.f };
	LightConstBuffer.PointLights[0].ColorAndFalloffExponent = {0.99f,0.5f,0.7f};

}

void Scene::Draw()
{
	D3D11DeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffer);
	D3D11DeviceContext->UpdateSubresource(ConstantBuffer, 0, 0, &LightConstBuffer, 0, 0);

	for (Camera* C : Cameras)
	{
		C->Render();
	}

	for (Mesh* M : Meshes)
	{
		M->Draw();
	}
}

void Scene::OnKeyDown(unsigned int KeyCode)
{
	switch (KeyCode)
	{
	case 'W':
	case 'w':
		if (CurrentCamera)
		{
			CurrentCamera->Walk(20.0f);
		}
		break;
	case 'A':
	case 'a':
		if (CurrentCamera)
		{
			CurrentCamera->Side(20.0f);
		}
		break;
	case 'D':
	case 'd':
		if (CurrentCamera)
		{
			CurrentCamera->Side(-20.0f);
		}
		break;
	case 'S':
	case 's':
		if (CurrentCamera)
		{
			CurrentCamera->Back(20.0f);
		}
		break;
	}
}

void Scene::OnKeyUp(unsigned int KeyCode)
{

}

void Scene::OnMouseDown(int X, int Y)
{
	if (CurrentCamera)
	{
		CurrentCamera->StartDrag(X,Y);
	}
}

void Scene::OnMouseUp(int X, int Y)
{
	if (CurrentCamera)
	{
		CurrentCamera->StopDrag(X, Y);
	}
}

void Scene::OnRightMouseDown(int X, int Y)
{
	if (CurrentCamera)
	{
		CurrentCamera->StartRotate(X, Y);
	}
}

void Scene::OnRightMouseUp(int X, int Y)
{
	if (CurrentCamera)
	{
		CurrentCamera->StopRotate(X, Y);
	}
}

void Scene::OnMouseMove(int X, int Y)
{
	if (CurrentCamera)
	{
		CurrentCamera->Drag(X, Y);
		CurrentCamera->Rotate(X, Y);
	}
}
