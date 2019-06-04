#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	void SetPosition(float fX, float fY, float fZ);
	void SetRotation(float fRoll, float fPitch, float fYall);
	void Roll(float fValue);
	void Pitch(float fValue);
	void Yall(float fValue);
	
protected:
	XMMATRIX GetWorldMatrix() 
	{ 
		return XMMatrixRotationRollPitchYawFromVector(Rotation) + XMMatrixTranslationFromVector(Position);
	}

	XMVECTOR Position;
	XMVECTOR Rotation;
};