#pragma once

#include "Vector.h"
#include "RayTracing.h"
#include "Bounds.h"

#include "DirectXMath.h"
using namespace DirectX;

inline bool operator==(const XMFLOAT4X4& lhs, const XMFLOAT4X4& rhs)
{
	return lhs._11 == rhs._11 && lhs._12 == rhs._12 && lhs._13 == rhs._13 && lhs._14 == rhs._14
		&& lhs._21 == rhs._21 && lhs._22 == rhs._22 && lhs._23 == rhs._23 && lhs._24 == rhs._24
		&& lhs._31 == rhs._31 && lhs._32 == rhs._32 && lhs._33 == rhs._33 && lhs._34 == rhs._34
		&& lhs._41 == rhs._41 && lhs._42 == rhs._42 && lhs._43 == rhs._43 && lhs._44 == rhs._44;
}
inline bool operator!=(const XMFLOAT4X4& lhs, const XMFLOAT4X4& rhs)
{
	return lhs._11 != rhs._11 || lhs._12 != rhs._12 || lhs._13 != rhs._13 || lhs._14 != rhs._14
		&& lhs._21 != rhs._21 || lhs._22 != rhs._22 || lhs._23 != rhs._23 || lhs._24 != rhs._24
		&& lhs._31 != rhs._31 || lhs._32 != rhs._32 || lhs._33 != rhs._33 || lhs._34 != rhs._34
		&& lhs._41 != rhs._41 || lhs._42 != rhs._42 || lhs._43 != rhs._43 || lhs._44 != rhs._44;
}
class Transform
{
public:
	Transform() {}
	Transform(const float mat[4][4]) 
	{
		M._11 = mat[0][0]; M._12 = mat[0][1]; M._13 = mat[0][2]; M._14 = mat[0][3];
		M._21 = mat[1][0]; M._22 = mat[1][1]; M._23 = mat[1][2]; M._24 = mat[1][3];
		M._31 = mat[2][0]; M._32 = mat[2][1]; M._33 = mat[2][2]; M._34 = mat[2][3];
		M._41 = mat[3][0]; M._42 = mat[3][1]; M._43 = mat[3][2]; M._44 = mat[3][3];

		XMVECTOR D;
		XMStoreFloat4x4(&InvM, XMMatrixInverse(&D, XMLoadFloat4x4(&M)));
	}
	Transform(const XMFLOAT4X4& m):M(m) 
	{ 
		XMVECTOR D;
		XMStoreFloat4x4(&InvM, XMMatrixInverse(&D, XMLoadFloat4x4(&M)));
	}
	Transform(const XMFLOAT4X4& m, const XMFLOAT4X4& Invm) :M(m),InvM(Invm) { }
	static Transform Identity()
	{
		XMFLOAT4X4 float4x4;
		XMStoreFloat4x4(&float4x4, XMMatrixIdentity());
		return Transform(float4x4);
	}
	static Transform Translate(float X, float Y, float Z)
	{
		XMFLOAT4X4 m;
		XMFLOAT4X4 minv;
		XMStoreFloat4x4(&m, XMMatrixTranslation(X, Y, Z));
		XMStoreFloat4x4(&minv, XMMatrixTranslation(-X, -Y, -Z));
		return Transform(m, minv);
	}
	static Transform Scale(float X, float Y, float Z)
	{
		XMFLOAT4X4 m;
		XMFLOAT4X4 minv;
		XMStoreFloat4x4(&m, XMMatrixScaling(X, Y, Z));
		XMStoreFloat4x4(&minv, XMMatrixScaling(1.f / X, 1.f / Y, 1.f / Z));
		return Transform(m, minv);
	}
	static Transform Rotate(float Pitch, float Yall, float Roll)
	{
		XMFLOAT4X4 float4x4;
		XMStoreFloat4x4(&float4x4, XMMatrixRotationRollPitchYaw(Pitch, Yall, Roll));
		return Transform(float4x4);
	}
	static Transform LookAt(const Vector3f& Eye, const Vector3f& At, const Vector3f& Up)
	{
		XMFLOAT4X4 float4x4;
		XMFLOAT3 XMEye = Eye.ToXMFloat3();
		XMFLOAT3 XMAt = At.ToXMFloat3();
		XMFLOAT3 XMUp = Up.ToXMFloat3();
		XMStoreFloat4x4(&float4x4, XMMatrixLookAtRH(XMLoadFloat3(&XMEye), XMLoadFloat3(&XMAt), XMLoadFloat3(&XMUp)));
		return Transform(float4x4);
	}
	static Transform Perspective(float fov, float aspect, float znear, float zfar)
	{
		XMFLOAT4X4 M;
		XMStoreFloat4x4(&M, XMMatrixPerspectiveFovRH(fov, aspect, znear, zfar));
		return Transform(M);
	}
	friend Transform Inverse(const Transform& t)
	{
		return Transform(t.InvM, t.M);
	}
	friend Transform Transpose(const Transform& t)
	{
		XMFLOAT4X4 M;
		XMFLOAT4X4 InvM;
		XMStoreFloat4x4(&M, XMMatrixTranspose(XMLoadFloat4x4(&t.M)));
		XMStoreFloat4x4(&InvM, XMMatrixTranspose(XMLoadFloat4x4(&t.InvM)));
		return Transform(M, InvM);
	}
	Bounds3f operator() (const Bounds3f& b) const;
	Transform operator*(const Transform& t);
	bool operator==(const Transform& t) const
	{
		return t.M == M && t.InvM == InvM;
	}
	bool operator!=(const Transform& t)
	{
		return t.M != M || t.InvM != InvM;
	}
	bool IsIdentity() const
	{
		return XMMatrixIsIdentity(XMLoadFloat4x4(&M));
	}
	template <typename T>
	inline Vector3<T> operator()(const Vector3<T>& p) const;
	template <typename T>
	inline Vector2<T> operator()(const Vector2<T>& p) const;
	template <typename T>
	inline Vector3<T> Normal(const Vector3<T>& p) const;
	inline Ray operator()(const Ray& r) const;
	SurfaceInteraction operator()(const SurfaceInteraction& si) const;
private:
	XMFLOAT4X4 M,InvM;
};

template<typename T>
inline Vector3<T> Transform::operator()(const Vector3<T>& p) const
{
	XMFLOAT4 P = { p.X,p.Y,p.Z, 1.f };
	XMVECTOR V = XMVector4Transform(XMLoadFloat4(&P), XMLoadFloat4x4(&M));
	XMStoreFloat4(&P,V);
	return Vector3<T>(P.x / P.w, P.y / P.w, P.z / P.w);
}

template <typename T>
inline Vector2<T> Transform::operator()(const Vector2<T>& p) const
{
	T x = p.X, y = p.Y, z = 0, w = 1.f;
	return Vector2<T>(	(M.m[0][0] * x + M.m[0][1] * y + M.m[0][2] * z + M.m[0][3] * w),
						(M.m[1][0] * x + M.m[1][1] * y + M.m[1][2] * z + M.m[1][3] * w));

	XMFLOAT3 P = { p.X,p.Y, 0.f };
	XMVECTOR V = XMVector3Transform(XMLoadFloat3(&P), XMLoadFloat4x4(&M));
	XMStoreFloat3(&P, V);
	return Vector2<T>(P.x, P.y);
}

template <typename T>
inline Vector3<T> Transform::Normal(const Vector3<T>& n) const
{
	XMFLOAT3 P = { n.X,n.Y,n.Z };
	XMFLOAT3X3 NormalT;
	XMStoreFloat3x3(&NormalT,XMMatrixTranspose(XMLoadFloat4x4(&InvM)));
	XMVECTOR V = XMVector3Transform(XMLoadFloat3(&P), XMLoadFloat3x3(&NormalT));
	XMStoreFloat3(&P, V);
	return Vector3<T>(P.x, P.y, P.z);

}

inline Ray Transform::operator()(const Ray& r) const
{
	Vector3f o = (*this)(r.o);
	Vector3f d = (*this).Normal(r.d);
	return Ray(o,d, r.tMax);
}
