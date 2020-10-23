#pragma once

#include "Poly2D.h"

#include <map>
#include <initializer_list>
#include <set>
#include <algorithm>

/*
�ж������Ƿ��ϰ����赲
*/
bool IsBlockByObstacles(const std::vector<Poly2D>& Obstacles, Vector2D P0, Vector2D P1)
{
	for (const Poly2D& Poly : Obstacles)
	{
		for (Edge2D Edge : Poly.Edges)
		{
			if (IsIntersect(P0, P1, Edge.Vertices[0], Edge.Vertices[1])) return true;
		}
	}
	return false;
}

/*
�����赲���Waypoints
@param Obstacles	�ϰ���
@param fExtents		�ϰ��ﶥ������
@param fMaxWalkableDistance ����������������,����ÿ�������ٽ���ķ�Χ
@return OutConvexPolys NevMesh͹��
*/
void CreateWayPoints(const std::vector<Poly2D>& Obstacles, float fExtents, float fMaxWalkableDistance, std::vector<Poly2D>& OutConvexPolys)
{
	//���ռ�����ζ��㼰��ӦNormal
	std::vector<Vector2D> ObstacleCorners;
	std::vector<std::vector<Vector2D>> ObstacleCornerNormals;
	for (const Poly2D& Poly : Obstacles)
	{
		for (size_t i = 0; i < Poly.Vertices.size(); ++i)
		{
			ObstacleCorners.push_back(Poly.Vertices[i]);
			ObstacleCornerNormals.emplace_back(std::vector<Vector2D>());
			for (const Edge2D& Edge : Poly.VertexToEdges[i])
			{
				ObstacleCornerNormals[i].push_back(Edge.Normal);
			}
		}
	}

	//����Extends����������һ��,
	std::vector<Vector2D> ObstacleCornersWithExtend;
	for (size_t i = 0; i < ObstacleCorners.size(); ++i)
	{
		Vector2D Normal(0.0f);
		for (const Vector2D& N : ObstacleCornerNormals[i])
		{
			Normal += N;
		}
		Vector2D Corner = ObstacleCorners[i];
		Normal.Normalize();
		Vector2D NewCorner = Corner + Normal * fExtents;
		ObstacleCornersWithExtend.push_back(NewCorner);
	}

	//����ÿ�����Ӷ������ٽ���Ĺ�ϵ������ɴ�(�м�û���ϰ���),���¼��ϵ
	std::vector<std::vector<size_t>> PointRelations;
	for (size_t i = 0; i < ObstacleCornersWithExtend.size(); ++i)
	{
		PointRelations.emplace_back(std::vector<size_t>());
		for (size_t j = 0; j < ObstacleCornersWithExtend.size(); ++j)
		{
			if (i == j) continue;

			const Vector2D& Pi = ObstacleCornersWithExtend[i];
			const Vector2D& Pj = ObstacleCornersWithExtend[j];

			if ((Pi - Pj).SizeSquared() >= fMaxWalkableDistance * fMaxWalkableDistance) continue;//�����������߾��������
			if (IsBlockByObstacles(Obstacles, Pi, Pj)) continue;//�м����赲������

			PointRelations[i].push_back(j);
		}
	}

	//���˹�����
	auto it = PointRelations.begin();
	auto it2 = ObstacleCornersWithExtend.begin();
	for (; it != PointRelations.end(); ++it, ++it2)
	{
		if (it->size() < 2)
		{
			it = PointRelations.erase(it);
			it2 = ObstacleCornersWithExtend.erase(it2);
		}
	}
}