#include "Actor.h"

Actor::Actor()
{
	Position = XMVectorSet(0, 0, 0, 0);
	Rotation = XMVectorSet(0, 0, 0, 0);
}

Actor::~Actor()
{

}

void Actor::SetPosition(float fX, float fY, float fZ)
{
	Position = XMVectorSet(fX, fY, fZ, 1);
}

void Actor::SetRotation(float fRoll, float fPitch, float fYall)
{
	Rotation = XMVectorSet(fRoll, fPitch, fYall, 1);
}

void Actor::Roll(float fValue)
{
	float fNewRoll = XMVectorGetX(Rotation) + fValue;
	XMVectorSetX(Rotation, fNewRoll);
}

void Actor::Pitch(float fValue)
{
	float fNewPitch = XMVectorGetY(Rotation) + fValue;
	XMVectorSetY(Rotation, fNewPitch);
}

void Actor::Yall(float fValue)
{
	float fNewYall = XMVectorGetZ(Rotation) + fValue;
	XMVectorSetZ(Rotation, fNewYall);
}

