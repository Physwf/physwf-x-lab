#include "Camera.h"
#include "log.h"

Camera::Camera():Near(1.0f), Far(1000.0f)
{
	ConstantBuffer = NULL;
	Up = XMVectorSet(0, 1, 0, 1);
}

void Camera::SetPostion(XMVECTOR Position)
{
	Eye = Position;
	UpdateViewMatrix();
}

void Camera::LookAt(XMVECTOR Target)
{
	LookAtTarget = Target;
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
	VSConstBuffer.ViewMatrix = XMMatrixLookAtLH(Eye,LookAtTarget,Up);
}

void Camera::UpdateProjMatrix()
{
	VSConstBuffer.ProjMatrix = XMMatrixPerspectiveFovLH(60, fViewportWidth / fViewportHeight, Near, Far);
	XMMATRIX
	(
		2*Near/fViewportWidth,	0,						0,						0,
		0,						2*Near/fViewportHeight,	0,						0,
		0,						0,						Far/(Far-Near),			-(Near*Far)/ (Far - Near),
		0,						0,						1,						0
	);
}
