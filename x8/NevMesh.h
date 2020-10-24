#pragma once

#include "Poly2D.h"
/*
 ����βü�
 @param ClipedPoly		���ü��Ķ����
 @param Clipers		    �����ü��Ķ����(���ϰ�������)
 @return ResultEdges	�ü��������ı�,��˳ʱ��˳��
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers, std::vector<std::vector<Vector2D>>& ResultPolys);
/*
 �Ӷ���ι���������Mesh
 ref:https://blog.csdn.net/ynnmnm/article/details/44833007
*/
void ConstructTriangleMesh(
	const std::vector<std::vector<Vector2D>>& InputPolys,
	std::vector<Vector2D>& AllVertices,
	std::vector<size_t>& OutTrianglesIndices
);