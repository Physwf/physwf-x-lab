#include "Poly2D.h"

Edge2D CreateEdge(Vector2D P0, Vector2D P1)
{
	Edge2D Edge;
	Edge.Vertices[0] = P0;
	Edge.Vertices[1] = P1;
	Vector2D Dir = P0 - P1;
	//https://zhidao.baidu.com/question/1866555603280150747.html
	Edge.Normal = Vector2D(-Dir.Y, Dir.X);//左侧,顺时针在外
	Edge.Normal.Normalize();
	return Edge;
}

Poly2D CreateTriangle(Vector2D P0, Vector2D P1, Vector2D P2)
{
	Poly2D Result;

	Edge2D Edge01 = CreateEdge(P0, P1);
	Edge2D Edge12 = CreateEdge(P1, P2);
	Edge2D Edge20 = CreateEdge(P2, P1);

	Result.Vertices.emplace_back(P0);
	Result.Vertices.emplace_back(P1);
	Result.Vertices.emplace_back(P2);

	Result.Edges.push_back(Edge01);
	Result.Edges.push_back(Edge12);
	Result.Edges.push_back(Edge20);

	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());

	Result.VertexToEdges[0].push_back(Edge01);
	Result.VertexToEdges[0].push_back(Edge20);
	Result.VertexToEdges[1].push_back(Edge01);
	Result.VertexToEdges[1].push_back(Edge12);
	Result.VertexToEdges[2].push_back(Edge12);
	Result.VertexToEdges[2].push_back(Edge20);

	return Result;
}

Poly2D CreateRect(Vector2D MinCorner, Vector2D MaxCorner, float fRotation)
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

	Result.Edges.push_back(Edge01);
	Result.Edges.push_back(Edge12);
	Result.Edges.push_back(Edge23);
	Result.Edges.push_back(Edge30);

	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());

	Result.VertexToEdges[0].push_back(Edge01);
	Result.VertexToEdges[0].push_back(Edge30);
	Result.VertexToEdges[1].push_back(Edge01);
	Result.VertexToEdges[1].push_back(Edge12);
	Result.VertexToEdges[2].push_back(Edge12);
	Result.VertexToEdges[2].push_back(Edge23);
	Result.VertexToEdges[3].push_back(Edge23);
	Result.VertexToEdges[3].push_back(Edge30);

	return Result;
}

Poly2D CreateQuad(Vector2D P0, Vector2D P1, Vector2D P2, Vector2D P3)
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

	Result.Edges.push_back(Edge01);
	Result.Edges.push_back(Edge12);
	Result.Edges.push_back(Edge23);
	Result.Edges.push_back(Edge30);

	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	Result.VertexToEdges.emplace_back(std::vector<Edge2D>());

	Result.VertexToEdges[0].push_back(Edge01);
	Result.VertexToEdges[0].push_back(Edge30);
	Result.VertexToEdges[1].push_back(Edge01);
	Result.VertexToEdges[1].push_back(Edge12);
	Result.VertexToEdges[2].push_back(Edge12);
	Result.VertexToEdges[2].push_back(Edge23);
	Result.VertexToEdges[3].push_back(Edge23);
	Result.VertexToEdges[3].push_back(Edge30);

	return Result;
}

Poly2D CreateConvex(const std::vector<Vector2D>& Points)
{
	Poly2D Result;

	if (Points.size() < 3) return Result;

	for (size_t i = 0; i < Points.size(); ++i)
	{
		Result.VertexToEdges.emplace_back(std::vector<Edge2D>());
	}

	for (size_t i = 1; i < Points.size(); ++i)
	{
		Vector2D P0 = Points[i - 1];
		Vector2D P1 = Points[i];

		Result.Vertices.push_back(P0);
		Edge2D Edge = CreateEdge(P0, P1);
		Result.VertexToEdges[i - 1].push_back(Edge);
		Result.VertexToEdges[i].push_back(Edge);
	}

	return Result;
}

void SortSegment(Vector2D& P0, Vector2D& P1)
{
	if (P0.X > P1.X)
	{
		Vector2D Temp = P0;
		P0 = P1;
		P1 = Temp;
	}
	else if (P0.X == P1.X)
	{
		if (P0.Y > P1.Y)
		{
			Vector2D Temp = P0;
			P0 = P1;
			P1 = Temp;
		}
	}
}

bool IsGreater(const Vector2D& P0, const Vector2D& P1)
{
	if (P0.X > P1.X) return true;
	if (P0.X == P1.X)
	{
		if (P0.Y > P1.Y) return true;
	}
	return false;
}

bool IsLess(const Vector2D& P0, const Vector2D& P1)
{
	if (P0.X < P1.X) return true;
	if (P0.X == P1.X)
	{
		if (P0.Y < P1.Y) return true;
	}
	return false;
}

bool IsOnClockwiseSide(const Vector2D& Compareer, const Vector2D& Comparee)
{
	return (Compareer ^ Comparee) < 0;
}

bool IsIntersect(Vector2D A, Vector2D B, Vector2D C, Vector2D D)
{
	SortSegment(A, B);
	SortSegment(C, D);

	if (IsGreater(A, C))
	{
		Vector2D TempA = A;
		Vector2D TempB = B;
		A = C;		B = D;
		C = TempA;	D = TempB;
	}

	Vector2D AB = B - A;
	Vector2D CD = D - C;
	//判断是否平行
	if ((AB ^ CD) == 0.f)
	{
		//判断是否重合或者部分重合
		//A-------------B
		//       C-------------D
		Vector2D BC = C - B;
		Vector2D CD = D - C;
		if ((BC ^ CD) == 0.f)//共线
		{
			//AB的起点小于等于线段CD的起点，但线段AB的终点（注意是终点）大于等于线段CD的起点
			if ((IsLess(A, C) && IsGreater(B, C)))
			{
				return true;
			}
		}
		return false;//平行 不共线或共线不重合
	}

	//根据叉积计算是否相交
	Vector2D AC = C - A;
	Vector2D AD = D - A;
	Vector2D CA = A - C;
	Vector2D CB = B - C;
	float fCrossACAB = AC ^ AB;
	float fCrossADAB = AD ^ AB;
	float fCrossCACD = CA ^ CD;
	float fCrossCBCD = CB ^ CD;
	if (fCrossACAB * fCrossADAB < 0.f && fCrossCACD*fCrossCBCD < 0.f)//=0说明顶点与另一线段相交
	{
		return true;
	}
	return false;
}

float CalcArea(const Vector2D& A, const Vector2D& B, const Vector2D& C)
{
	return (B - A) ^ (C - A);
}

Vector2D CalcIntersection(const Vector2D& A, const Vector2D& B, const Vector2D& C, const Vector2D& D)
{
	float k = std::fabs(CalcArea(A, B, C)) / std::fabs(CalcArea(A, B, D));
	return Vector2D((C.X + k * D.X) / (1 + k), (C.Y + k * D.Y) / (1 + k));
}

void CalcBoundingCircle(const Vector2D& A, const Vector2D& B, const Vector2D& C, Circle& OutCircle)
{
	float a = (A - B).Size();
	Vector2D AC = C - A;
	Vector2D BC = C - B;
	AC.Normalize();
	BC.Normalize();
	float cosa = AC | BC;
	float sina = std::sqrtf(1.0f - cosa * cosa);
	OutCircle.Radius = a / sina * 2.0f;

	float x1 = A.X, y1 = A.Y;
	float x2 = B.X, y2 = B.Y;
	float x3 = C.X, y3 = C.Y;

	float A1 = 2 * (x2 - x1);
	float B1 = 2 * (y2 - y1);
	float C1 = x2 * x2 + y2 * y2 - x1 * x1 - y1 * y1;
	float A2 = 2 * (x3 - x2);
	float B2 = 2 * (y3 - y2);
	float C2 = x3 * x3 + y3 * y3 - x2 * x2 - y2 * y2;

	OutCircle.Center.X = ((C1*B2) - (C2*B1)) / ((A1*B2) - (A2*B1));
	OutCircle.Center.Y = ((A1*C2) - (A2*C1)) / ((A1*B2) - (A2*B1));
}

float CalcAngle(const Vector2D& P1, const Vector2D& P, const Vector2D& P2)
{
	Vector2D V1 = P1 - P;
	Vector2D V2 = P2 - P;
	V1.Normalize();
	V2.Normalize();
	float dot = V1 | V2;
	return std::acosf(dot);
}

