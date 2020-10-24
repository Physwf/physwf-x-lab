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
//��һ��ʸ���Ĵ�ֱʸ��
inline Vector2D GetPerpendicularDir2D(Vector2D Input)
{
	return Vector2D(-Input.Y, Input.X); // ���, ˳ʱ������
}
/*
����nά�����Ĵ�ֱ����
*/
template <size_t n>
inline void GetPerpendicularDirnD(float Input[], float Output[])
{
	int i = 0;
	for (; i < n; i++)//lenofver��������
	{
		if (Input[i]) break;//�ڵ�һ���������ҵ�һ����Ϊ0����a[i]��
	}
	if (i == n)
	{
		return;//���ȫΪ0�����������Ϊ0������ ��һ��������������������ֱ
	}

	for (int j = 0; j < n; j++)
	{
		if (i != j) Output[j] = 1.f; //��b�г���Output[i]��ȫ��Ϊ1(��������Ա�).Output[i] = 0;
	}

	float sum = 0.f;

	for (int k = 0; k < n; k++)
	{
		if (i != k) sum += Input[k] * Output[k];
	}
	Output[i] = -sum / Input[i]; //Output[i] = -sum / Input[i];
}


Edge2D CreateEdge(Vector2D P0, Vector2D P1);

//���е�Poly2D�ĵ㶼��˳ʱ�뷽��
Poly2D CreateTriangle(Vector2D P0, Vector2D P1, Vector2D P2);
/*
MinCorner ���½�
MaxCorner ���Ͻ�
fRotation ��ת,����
*/
Poly2D CreateRect(Vector2D MinCorner, Vector2D MaxCorner, float fRotation);

Poly2D CreateQuad(Vector2D P0, Vector2D P1, Vector2D P2, Vector2D P3);
/*
���ݸ����㴴��͹��,�еĵ���ܲ�һ���ٱ���,��һ�������ڽ�����ڲ�
*/
Poly2D CreateConvex(const std::vector<Vector2D>& Points);

/*
���߶ε�������������,�������Ĵ�,���������,�������Ĵ�
@return P0ΪС�㣬P1Ϊ���
*/
void SortSegment(Vector2D& P0, Vector2D& P1);
/*
����Ƚ�,�����������
*/
bool IsGreater(const Vector2D& P0, const Vector2D& P1);
/*
����Ƚ�,�����������
*/
bool IsLess(const Vector2D& P0, const Vector2D& P1);
/*
 �ж��Ƿ���ʸ��Comparee�Ƿ���Compareer��˳ʱ�뷽��
*/
bool IsOnClockwiseSide(const Vector2D& Compareer, const Vector2D& Comparee);
/*
 �ж������߶��Ƿ��ཻ
 ref: https://www.cnblogs.com/tuyang1129/p/9390376.html
*/
bool IsIntersect(Vector2D A, Vector2D B, Vector2D C, Vector2D D);

//���������㹹�ɵ�������(������)���
float CalcArea(const Vector2D& A, const Vector2D& B, const Vector2D& C);

Vector2D CalcIntersection(const Vector2D& A, const Vector2D& B, const Vector2D& C, const Vector2D& D);
/*
 ���������ε���������,������Բ
*/
//https://baike.baidu.com/item/���Բ�뾶��ʽ/14818938
void CalcBoundingCircle(const Vector2D& A, const Vector2D& B, const Vector2D& C, Circle& OutCircle);
/*
 ����<P1PP2�ǵĽǶ�,�Ի���Ϊ��λ
		P1
	   /
	  /
   P /_________P2
*/
float CalcAngle(const Vector2D& P1, const Vector2D& P, const Vector2D& P2);