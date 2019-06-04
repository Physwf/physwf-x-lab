#include "Camera.h"
#include "log.h"

Camera::Camera()
{
	ConstantBuffer = NULL;
	Eye = XMVectorSet(0,0,0,1);
	LookDir = XMVectorSet(0, 0, 1, 1);
	Up = XMVectorSet(0, 1, 0, 1);
	UpdateViewMatrix();
}

void Camera::SetPostion(XMVECTOR Position)
{
	Eye = Position;
	UpdateViewMatrix();
}

void Camera::LookAt(XMVECTOR Target)
{
	if (XMVector3Equal(Target, Eye)) return;
	LookDir = Target - Eye;
	UpdateViewMatrix();
}

void Camera::SetViewport(float fWidth, float fHeight)
{
	fViewportWidth = fWidth;
	fViewportHeight = fHeight;
	UpdateProjMatrix();
}

void Camera::SetLen(float fNear, float fFar)
{
	Near = fNear;
	Far = fFar;
	UpdateProjMatrix();
}

void Camera::InitResource()
{
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VSConstBuffer;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = D3D11Device->CreateBuffer(&Desc, &InitData, &ConstantBuffer);
	if (FAILED(hr))
	{
		X_LOG("D3D11Device->CreateBuffer failed!");
		return;
	}
}

void Camera::ReleaseResource()
{
	if (ConstantBuffer)
	{
		ConstantBuffer->Release();
	}
}

void Camera::Render()
{
	D3D11DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	D3D11DeviceContext->UpdateSubresource(ConstantBuffer, 0, 0, &VSConstBuffer, 0, 0);
}

void Camera::UpdateViewMatrix()
{
	Up /= XMVectorGetByIndex(XMVector3Dot(Up, Up),0);
	LookDir /= XMVectorGetByIndex(XMVector3Dot(LookDir,LookDir), 0);
	XMVECTOR X = XMVector3Cross(Up, LookDir);
	X /= XMVectorGetByIndex(XMVector2Dot(X,X), 0);
	Up = XMVector3Cross(LookDir, X);
	uint32_t Zero[] = { 0,0,0,1 };
	VSConstBuffer.ViewMatrix = XMMATRIX(X, Up, LookDir, XMLoadInt4(Zero));
}

void Camera::UpdateProjMatrix()
{
	VSConstBuffer.ProjMatrix = XMMATRIX
	(
		2*Near/fViewportWidth,	0,						0,						0,
		0,						2*Near/fViewportHeight,	0,						0,
		0,						0,						Far/(Far-Near),			-(Near*Far)/ (Far - Near),
		0,						0,						1,						0
	);
}
