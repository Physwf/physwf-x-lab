#pragma once

#include <vector>
#include <map>
#include <initializer_list>
#include <set>

struct Vector2D
{
	float X, Y;

	Vector2D() : X(0.0f), Y(0.0f) {}
	explicit Vector2D(float V) : X(V), Y(V) {}
	Vector2D(float InX, float InY) : X(InX), Y(InY) {}
	Vector2D(std::initializer_list<float> list)
	{
		int i = 0;
		for (auto&& v : list)
		{
			(*this)[i] = v;
			if (i++ > 1) break;
		}
	}
	Vector2D operator+(const Vector2D& rhs) const { return { X + rhs.X, Y + rhs.Y }; }
	Vector2D operator-(const Vector2D& rhs) const { return { X - rhs.X, Y - rhs.Y }; }
	Vector2D operator*(const Vector2D& rhs) const { return { X * rhs.X, Y * rhs.Y }; }
	Vector2D operator*(float Value) const { return { X * Value, Y * Value }; }
	Vector2D operator/(float Value) const { return { X / Value, Y / Value }; }
	float operator^(const Vector2D& rhs) const { return X * rhs.Y - Y * rhs.X; }
	float operator|(const Vector2D& V) const { return X * V.X + Y * V.Y; }
	Vector2D operator+=(const Vector2D& rhs) { X += rhs.X; Y += rhs.Y; return *this; }
	Vector2D operator-=(const Vector2D& rhs) { X -= rhs.X; Y -= rhs.Y; return *this; }
	Vector2D operator*=(const Vector2D& rhs) { X *= rhs.X; Y *= rhs.Y; return *this; }
	Vector2D operator*=(float Value) { X *= Value; Y *= Value; return *this; }
	Vector2D operator/=(float Value) { X /= Value; Y /= Value; return *this; }
	Vector2D operator-() const { return { -X,-Y }; }
	bool operator==(const Vector2D& rhs) const { return (X != rhs.X || Y != rhs.Y); }
	bool operator!=(const Vector2D& rhs) const { return (X != rhs.X || Y != rhs.Y); }
	float& operator[](int Index) { return (&X)[Index]; }
	float operator[](int Index) const { return (&X)[Index]; }
	void Normalize() 
	{ 
		float InvSqrt = 1.0f / sqrtf(X * X + Y * Y);
		X *= InvSqrt;
		Y *= InvSqrt;
	}
	float Size() { return sqrtf(X * X + Y * Y); }
	float SizeSquared() { return X * X + Y * Y; }
	friend inline Vector2D operator*(float Value, const Vector2D& rhs) { return rhs.operator*(Value); }
};


struct Matrix2D
{
	float M[2][2];
};

struct Edge2D
{
	Vector2D Vertices[2];
	Vector2D Normal;
};

struct Poly2D
{
	std::map<int,Edge2D> Edges;		//Index -> Edge
	std::vector<Vector2D> Vertices;
};

Vector2D GetPerpendicularDir2D(Vector2D Input)
{
	return Vector2D(-Input.Y, Input.X); // 左侧, 顺时针在外
}
template <size_t n>
void GetPerpendicularDirnD(float Input[], float Output[])
{
	int i = 0;
	for (; i < n; i++)//lenofver向量长度
	{
		if (Input[i]) break;//在第一个向量中找到一个不为0的数a[i]，
	}
	if (i == n)
	{
		return;//如果全为0，则这个向量为0向量， 任一个向量都于所求向量垂直
	}

	for (int j = 0; j < n; j++)
	{
		if (i != j) Output[j] = 1.f; //令b中除了Output[i]外全置为1(这个数可以变).Output[i] = 0;
	}

	float sum = 0.f;

	for (int k = 0; k < n; k++)
	{
		if (i != k) sum += Input[k] * Output[k];
	}
	Output[i] = -sum / Input[i]; //Output[i] = -sum / Input[i];
}


inline Edge2D CreateEdge(Vector2D P0, Vector2D P1)
{
	Edge2D Edge;
	Edge.Vertices[0] = P0;
	Edge.Vertices[1] = P1;
	Vector2D Dir = P0 - P1;
	//https://zhidao.baidu.com/question/1866555603280150747.html
	Edge.Normal = Vector2D(-Dir.Y,Dir.X);//左侧,顺时针在外
	Edge.Normal.Normalize();
}

//所有的Poly2D的点都是顺时针方向
inline Poly2D CreateTriangle(Vector2D P0, Vector2D P1, Vector2D P2)
{
	Poly2D Result;

	Edge2D Edge01 = CreateEdge(P0, P1);
	Edge2D Edge12 = CreateEdge(P1, P2);
	Edge2D Edge20 = CreateEdge(P2, P1);

	Result.Vertices.emplace_back(P0);
	Result.Vertices.emplace_back(P1);
	Result.Vertices.emplace_back(P2);

	Result.Edges.insert(std::make_pair(0, Edge01));
	Result.Edges.insert(std::make_pair(0, Edge20));
	Result.Edges.insert(std::make_pair(1, Edge12));
	Result.Edges.insert(std::make_pair(1, Edge01));
	Result.Edges.insert(std::make_pair(2, Edge12));
	Result.Edges.insert(std::make_pair(2, Edge20));

	return Result;
}
/*
	MinCorner 左下角
	MaxCorner 右上角
	fRotation 旋转,弧度
*/
inline Poly2D CreateRect(Vector2D MinCorner, Vector2D MaxCorner,float fRotation)
{
	Poly2D Result;

	Vector2D P0 = Vector2D(MinCorner.X, MaxCorner.Y);	//左上角
	Vector2D P1 = MaxCorner;							//右上角
	Vector2D P2 = Vector2D(MaxCorner.X, MinCorner.Y);	//右下角
	Vector2D P3 = MinCorner;							//左下角

	Edge2D Edge01 = CreateEdge(P0, P1);
	Edge2D Edge12 = CreateEdge(P1, P2);
	Edge2D Edge23 = CreateEdge(P2, P3);
	Edge2D Edge30 = CreateEdge(P3, P0);

	Result.Vertices.emplace_back(P0);
	Result.Vertices.emplace_back(P1);
	Result.Vertices.emplace_back(P2);
	Result.Vertices.emplace_back(P3);

	Result.Edges.insert(std::make_pair(0, Edge01));
	Result.Edges.insert(std::make_pair(0, Edge30));
	Result.Edges.insert(std::make_pair(1, Edge01));
	Result.Edges.insert(std::make_pair(1, Edge12));
	Result.Edges.insert(std::make_pair(2, Edge12));
	Result.Edges.insert(std::make_pair(2, Edge23));
	Result.Edges.insert(std::make_pair(3, Edge23));
	Result.Edges.insert(std::make_pair(3, Edge30));

	return Result;
}

inline Poly2D CreateQuad(Vector2D P0, Vector2D P1, Vector2D P2, Vector2D P3)
{
	Poly2D Result;

	Edge2D Edge01 = CreateEdge(P0, P1);
	Edge2D Edge12 = CreateEdge(P1, P2);
	Edge2D Edge23 = CreateEdge(P2, P3);
	Edge2D Edge30 = CreateEdge(P3, P0);

	Result.Vertices.emplace_back(P0);
	Result.Vertices.emplace_back(P1);
	Result.Vertices.emplace_back(P2);
	Result.Vertices.emplace_back(P3);

	Result.Edges.insert(std::make_pair(0, Edge01));
	Result.Edges.insert(std::make_pair(0, Edge30));
	Result.Edges.insert(std::make_pair(1, Edge01));
	Result.Edges.insert(std::make_pair(1, Edge12));
	Result.Edges.insert(std::make_pair(2, Edge12));
	Result.Edges.insert(std::make_pair(2, Edge23));
	Result.Edges.insert(std::make_pair(3, Edge23));
	Result.Edges.insert(std::make_pair(3, Edge30));

	return Result;
}
/*
	根据给定点创建凸包,有的点可能不一定再边上,但一定包含在结果的内部
*/
inline Poly2D CreateConvex(const std::vector<Vector2D>& Points)
{
	Poly2D Result;

	if (Points.size() < 3) return Result;

	for (size_t i=1;i<Points.size();++i)
	{
		Vector2D P0 = Points[i - 1];
		Vector2D P1 = Points[i];

		Result.Vertices.push_back(P0);
		Edge2D Edge = CreateEdge(P0, P1);
		Result.Edges.insert(std::make_pair(i - 1, Edge));
		Result.Edges.insert(std::make_pair(i,	  Edge));
	}

	return Result;
}

/*
根据阻挡算出NevMesh
*/
void CreateNevMesh2D(const std::vector<Poly2D>& Obstacles,float fExtents, std::vector<Poly2D>& OutConvexPolys)
{
	//先根据阻挡多边形的顶点算出
	std::vector<Vector2D> WalkablePoints;
	std::vector<std::vector<Vector2D>> WalkableNormals;
	for (const Poly2D& Poly : Obstacles)
	{
		for (size_t i = 0; i < Poly.Edges.size(); ++i)
		{
			//Edge2D Edgei = Poly.Edges[i];
			//WalkablePoints.insert(Pi);
		}
	}
}