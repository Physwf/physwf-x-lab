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
}

void Scene::Setup()
{
	Mesh* M = new Mesh();
	M->ImportFromFBX("./4gikevqde8-IronMan/ironman.fbx");
	M->InitResource();
	Meshes.push_back(M);

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

	for (Mesh* M : Meshes)
	{
		M->Draw();
	}
}
