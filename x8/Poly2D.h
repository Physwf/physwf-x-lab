#pragma once

#include <vector>
#include "Math2D.h"

struct Edge2D
{
	Vector2D Vertices[2];
	Vector2D Normal;
};

struct Poly2D
{
	std::vector<std::vector<Edge2D>>	VertexToEdges;		//Index -> Edge
	std::vector<Vector2D>				Vertices;
	std::vector<Edge2D>					Edges;

	bool operator==(const Poly2D& rhs)
	{
		if (Vertices.size() == rhs.Vertices.size())
		{
			for (const Vector2D& P0 : Vertices)
			{
				bool bEqual = false;
				for (const Vector2D& P1 : rhs.Vertices)
				{
					if (P0 == P1) bEqual = true;
				}
				if (!bEqual) return false;
			}
			return true;
		}
		return false;
	}
};
//求一个矢量的垂直矢量
inline Vector2D GetPerpendicularDir2D(Vector2D Input)
{
	return Vector2D(-Input.Y, Input.X); // 左侧, 顺时针在外
}
/*
计算n维向量的垂直向量
*/
template <size_t n>
inline void GetPerpendicularDirnD(float Input[], float Output[])
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


Edge2D CreateEdge(Vector2D P0, Vector2D P1);

//所有的Poly2D的点都是顺时针方向
Poly2D CreateTriangle(Vector2D P0, Vector2D P1, Vector2D P2);
/*
MinCorner 左下角
MaxCorner 右上角
fRotation 旋转,弧度
*/
Poly2D CreateRect(Vector2D MinCorner, Vector2D MaxCorner, float fRotation);

Poly2D CreateQuad(Vector2D P0, Vector2D P1, Vector2D P2, Vector2D P3);
/*
根据给定点创建凸包,有的点可能不一定再边上,但一定包含在结果的内部
*/
Poly2D CreateConvex(const std::vector<Vector2D>& Points);

/*
给线段的两个顶点排序,横坐标大的大,横坐标相等,纵坐标大的大
@return P0为小点，P1为大点
*/
void SortSegment(Vector2D& P0, Vector2D& P1);
/*
顶点比较,基于排序规则
*/
bool IsGreater(const Vector2D& P0, const Vector2D& P1);
/*
顶点比较,基于排序规则
*/
bool IsLess(const Vector2D& P0, const Vector2D& P1);
/*
 判断是否在矢量Comparee是否在Compareer的顺时针方向
*/
bool IsOnClockwiseSide(const Vector2D& Compareer, const Vector2D& Comparee);
/*
 判断两个线段是否相交
 ref: https://www.cnblogs.com/tuyang1129/p/9390376.html
*/
bool IsIntersect(Vector2D A, Vector2D B, Vector2D C, Vector2D D);

//计算三个点构成的三角形(带符号)面积
float CalcArea(const Vector2D& A, const Vector2D& B, const Vector2D& C);

Vector2D CalcIntersection(const Vector2D& A, const Vector2D& B, const Vector2D& C, const Vector2D& D);
/*
 给定三角形的三个顶点,求出外接圆
*/
//https://baike.baidu.com/item/外接圆半径公式/14818938
void CalcBoundingCircle(const Vector2D& A, const Vector2D& B, const Vector2D& C, Circle& OutCircle);
/*
 计算<P1PP2角的角度,以弧度为单位
		P1
	   /
	  /
   P /_________P2
*/
float CalcAngle(const Vector2D& P1, const Vector2D& P, const Vector2D& P2);