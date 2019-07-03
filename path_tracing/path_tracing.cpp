#include <math.h>
#include <stdlib.h>

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
	Vec Multiply(const Vec& rhs)
	{
		return { Y * rhs.Z - Z * rhs.Y, X * rhs.Z - Z * rhs.X, X*rhs.Y - Y * rhs.X };
	}
	Vec operator/(float fValue)
	{
		return { X / fValue, Y / fValue, Z / fValue };
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

	bool operator==(const Vec& rhs)
	{
		return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
	}
};

Vec operator*(float fValue , const Vec& InVec)
{
	return { fValue*InVec.X, fValue*InVec.Y,  fValue*InVec.Z };
}

struct Color
{
	float R, G, B;
	Color operator+(const Color& rhs) const
	{
		return { R+rhs.R, G+rhs.G, B+rhs.B };
	}
	Color operator*(const Color& rhs) const
	{
		return { R*rhs.R, G*rhs.G, B*rhs.B };
	}
	Color operator/(float fValue) const
	{
		return { R / fValue, G / fValue, B / fValue };
	}
	static Color Black;
	static Color White;
};

Color operator*(float fValue, const Color& InVec)
{
	return { fValue*InVec.R, fValue*InVec.G,  fValue*InVec.B };
}

Color Color::Black = { 0.0f,0.0f,0.0f };
Color Color::White = { 1.0f,1.0f,1.0f };

struct Ray
{
	Vec Position;
	Vec Dir;
};


struct Material
{
	Color Emittance;
	Color Reflectance;
};

struct HitResult
{
	bool bHitted;
	float t;
	Material M;
	Vec HitPoint;
	Vec Normal;
};

struct Square 
{
	Material M;
	Vec Center;
	Vec Normal;

	float Width;

	bool Intersect(const Ray& InRay, HitResult& Result) const
	{
		Result.bHitted = false;
		float fDenominator = InRay.Dir * Normal;
		if (fabsf(fDenominator) <= 0.00000001f) return false;
		Result.t = (Center - InRay.Position)*Normal / fDenominator;
		Result.HitPoint =
		{ 
			InRay.Position.X + InRay.Dir.X * Result.t,
			InRay.Position.Y + InRay.Dir.Y * Result.t,
			InRay.Position.Z + InRay.Dir.Z * Result.t
		};
		float fHalfW = Width / 2.0f;
		Result.M = M;
		Result.Normal = Normal;
		Result.bHitted = (fabsf(Result.HitPoint.X - Center.X) < fHalfW) && (fabsf(Result.HitPoint.Y - Center.Y) < fHalfW) && (fabsf(Result.HitPoint.Z - Center.Z) < fHalfW);
		return Result.bHitted;
	}
};

struct Sphere
{
	Material M;
	Vec Position;
	float fRadius;

	bool Intersect(const Ray& InRay, HitResult& Result) const
	{
		///https://en.wikipedia.org/wiki/Line-sphere_intersection
		Result.bHitted = false;
		Vec PositionVec = InRay.Position - Position;
		float fValue = InRay.Dir*PositionVec;
		float fLen = PositionVec.Length();
		float fPart2 = fValue * fValue - (fLen * fLen - fRadius * fRadius);
		if (fPart2 < 0.0f) return false;
		float fPart1 = -(InRay.Dir*PositionVec);
		//float t1 = fPart1 + fPart2;
		Result.bHitted = true;
		Result.M = M;
		Result.t = fPart1 - fPart2;
		Result.HitPoint = InRay.Position + Result.t * InRay.Dir;
		Result.Normal = Result.HitPoint - Position;
		return true;
	}
};

struct Circle
{
	Material M;
	Vec Center;
	float fRadius;
	Vec Normal;

	bool Intersect(const Ray& InRay, HitResult& Result) const
	{
		Result.bHitted = false;
		float fDenominator = InRay.Dir * Normal;
		if (fabsf(fDenominator) <= 0.00000001f) return false;
		Result.t = (Center - InRay.Position)*Normal / fDenominator;
		Result.HitPoint =
		{
			InRay.Position.X + InRay.Dir.X * Result.t,
			InRay.Position.Y + InRay.Dir.Y * Result.t,
			InRay.Position.Z + InRay.Dir.Z * Result.t
		};
		Result.M = M;
		Result.Normal = Normal;
		Result.bHitted = (Center - Result.HitPoint).Length() < fRadius;
		return Result.bHitted;
	}
};

struct Camera
{
	Vec Eye;
	Vec LookAtTarget;
	Vec Up;

	float Len;
	float Width;
	float Height;

	void SetPosition(Vec InPos) { Eye = InPos; }
	void SetUp(Vec InUp)  { Up = InUp; }
	void SetViewport(float W, float H) { Width = W; Height = H; }
	void LookAt(Vec Point) { LookAtTarget = Point; }
	void SetLen(float fValue) { Len = fValue; }

	Ray GetPixelRay(int iPixelX, int iPixelY)
	{
		Vec Dir = LookAtTarget - Eye;
		Dir.Normalize();
		Vec ScreenCenter = Len*Dir;
		Vec HorizentalVec = Dir.Multiply(Up);
		HorizentalVec.Normalize();
		Ray Result;
		Result.Position = Eye;
		Result.Dir = (ScreenCenter + (iPixelX - Width / 2)*HorizentalVec + ( Height / 2 - iPixelY)*Up) - Eye;
		Result.Dir.Normalize();
		return Result;
	}
};

struct Scene
{
	Square Walls[5];
	Sphere Balls[1];
	Circle Lights[1];

	Camera C;

	HitResult Trace(const Ray& InRay)
	{
		HitResult Result;
		Result.bHitted = false;
		Result.t = 100000000;
		HitResult Temp;

		for (Circle& l : Lights)
		{
			if (l.Intersect(InRay, Temp))
			{
				if (Result.t > Temp.t)
				{
					Result = Temp;
				}
			}
		}

		for (Sphere& b : Balls)
		{
			if (b.Intersect(InRay, Temp))
			{
				if (Result.t > Temp.t)
				{
					Result = Temp;
				}
			}
		}

		for (Square& o : Walls)
		{
			if (o.Intersect(InRay, Temp))
			{
				if (Result.t > Temp.t)
				{
					Result = Temp;
				}
			}
		}

		return Result;
	}

	Vec RandomUnitVectorInHemisphereOf(Vec Normal)
	{
		Vec RandomVec = { rand() / (RAND_MAX + 1.0f),rand() / (RAND_MAX + 1.0f),rand() / (RAND_MAX + 1.0f) };
		if (RandomVec == Normal) return RandomUnitVectorInHemisphereOf(Normal);
		Normal.Normalize();
		RandomVec.Normalize();
		Vec Tangent = Normal.Multiply(RandomVec);
		Tangent.Normalize();
		float theta = (rand() / (RAND_MAX + 1.0f))*0.5f * 3.1415926f;
		float costheta = cosf(theta);
		return (costheta * Normal) + (1.0f - costheta)*(Normal*Tangent)*Tangent;// +sinf(thda) * (Tangent.Multiply(Normal));
	}

	Color Radiance(const Ray& InRay, int Depth)
	{
		if (Depth > 10)
		{
			return Color::Black;
		}

		HitResult Result = Trace(InRay);
		if (!Result.bHitted)
		{
			return Color::Black;
		}

		Ray newRay;
		newRay.Position = Result.HitPoint;
		newRay.Dir = RandomUnitVectorInHemisphereOf(Result.Normal);
		newRay.Dir.Normalize();

		const float p = 1 / (2 * 3.1415926f);

		float cos_theta = newRay.Dir * Result.Normal;
		Color BRDF = Result.M.Reflectance / 3.1415926f;

		Color Incomming = Radiance(newRay, Depth + 1);

		return Result.M.Emittance + ((cos_theta / p) * BRDF * Incomming );
	}
};

Scene S;

void SetUpScene()
{
	S.C.SetPosition({ 0,250,-500 });
	S.C.SetUp({ 0.0f,1.0f,0.0f });
	S.C.SetViewport(500, 500);
	S.C.LookAt({ 0,250,250 });
	S.C.SetLen(400.0f);

	S.Walls[0].Center = {0,250,500};
	S.Walls[0].Normal = { 0,0,-1.0f };
	S.Walls[0].Width = 500;
	S.Walls[0].M.Emittance = { 0,0,0 };
	S.Walls[0].M.Reflectance = { 0.4f,0.5f,0.4f };

	S.Walls[1].Center = { -250,250,250 };
	S.Walls[1].Normal = { 1.0f ,0,.0f };
	S.Walls[1].Width = 500;
	S.Walls[1].M.Emittance = { 0,0,0 };
	S.Walls[1].M.Reflectance = { 0.4f,0.5f,0.4f };

	S.Walls[2].Center = { 0,500,250 };
	S.Walls[2].Normal = { 0,-1,0 };
	S.Walls[2].Width = 500;
	S.Walls[2].M.Emittance = { 0,0,0 };
	S.Walls[2].M.Reflectance = { 0.4f,0.5f,0.4f };

	S.Walls[3].Center = { 250,250,250 };
	S.Walls[3].Normal = { -1.0f, 0,.0f };
	S.Walls[3].Width = 500;
	S.Walls[3].M.Emittance = { 0,0,0 };
	S.Walls[3].M.Reflectance = { 0.4f,0.5f,0.4f };

	S.Walls[4].Center = { 0,0.0f,250.0f };
	S.Walls[4].Normal = { 0, 1.0f, 0.0f };
	S.Walls[4].Width = 500;
	S.Walls[4].M.Emittance = { 0,0,0 };
	S.Walls[4].M.Reflectance = { 0.4f,0.5f,0.4f };

	S.Balls[0].Position = { 0,250,250 };
	S.Balls[0].fRadius = 80.0f;
	S.Balls[0].M.Emittance = { 0,0,0 };
	S.Balls[0].M.Reflectance = { 0.8f,0.7f,0.7f };

	S.Lights[0].Center = { 0,250,250 };
	S.Lights[0].fRadius = 200;
	S.Lights[0].Normal = { 0,-1,0 };
	S.Lights[0].M.Emittance = { 1,1,1 };
	S.Lights[0].M.Reflectance = { 0,0,0 };
}

void Render(int &W, int &H, unsigned int** Colors)
{
	W = 500;
	H = 500;
	unsigned int* Pixel = new unsigned int[W*H];
	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			//Ray R = S.C.GetPixelRay(250, 10);
			Ray R = S.C.GetPixelRay(x, y);
			Color C = { 0,0,0 };
			for (int i = 0; i < 8; ++i)
			{
				C = C + S.Radiance(R, 0);
			}
			C = C / 8.0f;
			Pixel[y * 500 + x] = C.R*255 * 0x10000 + C.G * 255 * 0x100 + C.B * 255 ;
		}
	}
	*Colors = Pixel;
}