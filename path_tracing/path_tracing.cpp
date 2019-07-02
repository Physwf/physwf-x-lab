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

	bool Intersect(const Ray& InRay,Vec OutPosition) const
	{
// 		Vec Dir1 = InRay.Position - Center;
// 		if (Dir1*InRay.Dir < 0) return false;
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

	bool Intersect(const Ray& InRay, Vec OutPosition) const
	{

	}
};

struct Light
{

};

struct Scene
{
	Square Walls[5];
	Sphere Ball[1];

	Vec Radiance(Ray InRay)
	{

	}
};