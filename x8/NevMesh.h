#pragma once

#include "Poly2D.h"

/*
 Weiler-Atherton多边形裁剪算法
 ref:https://blog.csdn.net/liaojinyu282/article/details/6011177
 @param ClipedPoly		被裁减的多边形
 @param Clipers			用来裁剪的多边形
 @return Result			裁剪结果,多个多边形
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<Vector2D>& Cliper, std::vector<std::vector<Vector2D>>& ResultEdges);
/*
  多边形裁剪
  @param ClipedPoly		被裁减的多边形
  @param Clipers		用来裁剪的多边形(用障碍物生成)
  @return ResultEdges	裁剪后连续的边,按顺时针顺序
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers,std::vector<std::vector<Vector2D>>& ResultEdges);

/*
 从多边形构建三角形Mesh
 ref:https://blog.csdn.net/ynnmnm/article/details/44833007
*/
void ConstructTriangleMesh(const std::vector<std::vector<Vector2D>>& InputPolys, std::vector<std::vector<Vector2D>>& OutTriangles);