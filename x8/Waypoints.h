#pragma once

#include "Poly2D.h"

#include <map>
#include <initializer_list>
#include <set>
#include <algorithm>

/*
判断两点是否被障碍物阻挡
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
根据阻挡算出Waypoints
@param Obstacles	障碍物
@param fExtents		障碍物顶点外延
@param fMaxWalkableDistance 可行走区域最大距离,决定每个点检查临近点的范围
@return OutConvexPolys NevMesh凸包
*/
void CreateWayPoints(const std::vector<Poly2D>& Obstacles, float fExtents, float fMaxWalkableDistance, std::vector<Poly2D>& OutConvexPolys)
{
	//先收集多边形顶点及对应Normal
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

	//根据Extends将顶点外延一点,
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

	//计算每个外延顶点与临近点的关系，如果可达(中间没有障碍物),则记录关系
	std::vector<std::vector<size_t>> PointRelations;
	for (size_t i = 0; i < ObstacleCornersWithExtend.size(); ++i)
	{
		PointRelations.emplace_back(std::vector<size_t>());
		for (size_t j = 0; j < ObstacleCornersWithExtend.size(); ++j)
		{
			if (i == j) continue;

			const Vector2D& Pi = ObstacleCornersWithExtend[i];
			const Vector2D& Pj = ObstacleCornersWithExtend[j];

			if ((Pi - Pj).SizeSquared() >= fMaxWalkableDistance * fMaxWalkableDistance) continue;//超过最大可行走距离的跳过
			if (IsBlockByObstacles(Obstacles, Pi, Pj)) continue;//中间有阻挡的跳过

			PointRelations[i].push_back(j);
		}
	}

	//过滤孤立点
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