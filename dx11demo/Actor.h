#pragma once

#include "Math.h"

class Actor
{
public:
	Actor();
	virtual ~Actor();

	void SetPosition(float fX, float fY, float fZ);
	void SetRotation(float fRoll, float fPitch, float fYall);
	void Pitch(float fValue);
	void Yall(float fValue);
	void Roll(float fValue);
	
protected:
	Matrix GetWorldMatrix() 
	{ 
		return Matrix::DXFormRotation(Rotation) * Matrix::DXFromTranslation(Position);
	}

	Vector Position;
	Vector Rotation;
};