#pragma once

#include "Poly2D.h"
/*
 多边形裁剪
 @param ClipedPoly		被裁减的多边形
 @param Clipers		    用来裁剪的多边形(用障碍物生成)
 @return ResultEdges	裁剪后连续的边,按顺时针顺序
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers, std::vector<std::vector<Vector2D>>& ResultPolys);
/*
 从多边形构建三角形Mesh
 ref:https://blog.csdn.net/ynnmnm/article/details/44833007
*/
void ConstructTriangleMesh(
	const std::vector<std::vector<Vector2D>>& InputPolys,
	std::vector<Vector2D>& AllVertices,
	std::vector<size_t>& OutTrianglesIndices
);