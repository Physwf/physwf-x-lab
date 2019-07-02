#include <math.h>

struct Vec
{
	float X, Y, Z;
	Vec operator+(const Vec& rhs) const
	{
		return { X + rhs.X, Y + rhs.Y, Z + rhs.Z };
	}
	Vec operator-(const Vec& rhs) const
	{
		return {X - rhs.X, Y - rhs.Y, Z - rhs.Z};
	}
	float operator*(const Vec& rhs) const
	{
		return X * rhs.X+ Y * rhs.Y+ Z * rhs.Z;
	}
	float Length() const
	{
		return sqrtf(X * X + Y * Y + Z * Z);
	}

	void Normalize()
	{
		float l = Length();
		X /= l; Y /= l; Z /= l;
	}
};

Vec operator*(float fValue , const Vec& InVec)
{
	return { fValue*InVec.X, fValue*InVec.Y,  fValue*InVec.Y };
}

struct Color
{
	float R, G, B;

	static Color Black = { 0.0f,0.0f,0.0f };
	static Color White = { 1.0f,1.0f,1.0f };
};

struct Ray
{
	Vec Position;
	Vec Dir;
};

struct Material
{

};

struct Square 
{
	Material M;
	Vec Center;
	Vec Normal;

	float Width;

	bool Intersect(const Ray& InRay,Vec& OutPosition) const
	{
		float fDenominator = InRay.Dir * Normal;
		if (fDenominator <= 0.00001f) return false;
		float t = (Center - InRay.Position)*Normal / fDenominator;
		OutPosition =
		{ 
			InRay.Position.X + InRay.Dir.X * t,
			InRay.Position.Y + InRay.Dir.Y * t, 
			InRay.Position.Z + InRay.Dir.Z * t 
		};
		float fHalfW = Width / 2.0f;
		return (fabsf(OutPosition.X - Center.X) < fHalfW) && (fabsf(OutPosition.Y - Center.Y) < fHalfW) && (fabsf(OutPosition.Z - Center.Z) < fHalfW);
	}
};

struct Sphere
{
	Material M;
	Vec Position;
	float fRadius;

	bool Intersect(const Ray& InRay, Vec& OutPosition) const
	{
		///https://en.wikipedia.org/wiki/Line-sphere_intersection
		Vec PositionVec = InRay.Position - Position;
		float fValue = InRay.Dir*PositionVec;
		float fLen = PositionVec.Length();
		float fPart2 = fValue * fValue - (fLen * fLen - fRadius * fRadius);
		if (fPart2 < 0.0f) return false;
		float fPart1 = -(InRay.Dir*PositionVec);
		//float t1 = fPart1 + fPart2;
		float t2 = fPart1 - fPart2;
		OutPosition = InRay.Position + t2 * InRay.Dir;
	}
};

struct Light
{

};

struct Scene
{
	Square Walls[5];
	Sphere Ball[1];



	Color Radiance(Ray InRay)
	{

	}
};