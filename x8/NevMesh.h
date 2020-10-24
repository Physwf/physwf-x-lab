#pragma once

#include "Poly2D.h"

/*
 Weiler-Atherton����βü��㷨
 ref:https://blog.csdn.net/liaojinyu282/article/details/6011177
 @param ClipedPoly		���ü��Ķ����
 @param Clipers			�����ü��Ķ����
 @return Result			�ü����,��������
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<Vector2D>& Cliper, std::vector<std::vector<Vector2D>>& ResultEdges);
/*
  ����βü�
  @param ClipedPoly		���ü��Ķ����
  @param Clipers		�����ü��Ķ����(���ϰ�������)
  @return ResultEdges	�ü��������ı�,��˳ʱ��˳��
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers,std::vector<std::vector<Vector2D>>& ResultEdges);

/*
 �Ӷ���ι���������Mesh
 ref:https://blog.csdn.net/ynnmnm/article/details/44833007
*/
void ConstructTriangleMesh(const std::vector<std::vector<Vector2D>>& InputPolys, std::vector<std::vector<Vector2D>>& OutTriangles);