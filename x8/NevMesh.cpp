#include "NevMesh.h"

#include <stack>
#include <queue>

//forward declaration
void TranvserCliper(std::vector<char>& bIsVisited, std::vector<size_t>& CombinedIndices, std::vector<char>& bIsIntersection, size_t StartIndex, size_t ClipedPolyVertexCount, size_t CliperVertexCount, std::vector<size_t>& ClipResult);

//�������ü��Ķ����
void TranvserCliped(
	std::vector<char>& bIsVisited, 
	std::vector<size_t>& CombinedIndices,
	std::vector<char>& bIsIntersection, 
	size_t StartIndex, 
	size_t ClipedPolyVertexCount,
	size_t CliperVertexCount,
	std::vector<size_t>& ClipResult
)
{
	auto it = CombinedIndices.begin() + StartIndex;
	for (size_t i = 0; i < ClipedPolyVertexCount; ++i, ++it)
	{
		if (bIsVisited[*it]) break;
		bIsVisited[*it] = true;
		ClipResult.push_back(*it);
		if (bIsIntersection[*it])//entering intersection
		{
			TranvserCliper(bIsVisited, CombinedIndices, bIsIntersection, *(++it), ClipedPolyVertexCount, CliperVertexCount, ClipResult);
			return;
		}
	}
}
//���������ü��Ķ����
void TranvserCliper(
	std::vector<char>& bIsVisited, 
	std::vector<size_t>& CombinedIndices,
	std::vector<char>& bIsIntersection,
	size_t StartIndex, 
	size_t ClipedPolyVertexCount,
	size_t CliperVertexCount,
	std::vector<size_t>& ClipResult
)
{
	auto it = CombinedIndices.begin() + StartIndex;
	for (size_t i = 0; i < CliperVertexCount; ++i, ++it)
	{
		if (bIsVisited[*it]) break;
		bIsVisited[*it] = true;
		ClipResult.push_back(*it);
		if (bIsIntersection[*it])//exit intersection
		{
			TranvserCliped(bIsVisited, CombinedIndices, bIsIntersection, *(++it), ClipedPolyVertexCount, CliperVertexCount, ClipResult);
			return;
		}
	}
}
/*
Weiler-Atherton����βü��㷨
ref:https://blog.csdn.net/liaojinyu282/article/details/6011177
@param ClipedPoly		���ü��Ķ����
@param Clipers			�����ü��Ķ����
@return Result			�ü����,��������
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<Vector2D>& Cliper, std::vector<std::vector<Vector2D>>& ResultPolys)
{
	if (ClipedPoly.size() < 3 || Cliper.size() < 3) return;

	//�����ж����һ��,����Ч��
	std::vector<Vector2D> AllPoints(ClipedPoly);
	AllPoints.insert(AllPoints.end(), Cliper.begin(), Cliper.end());
	//��������
	std::vector<size_t> ClipedPolyIndices;
	std::vector<size_t> CliperIndices;
	for (size_t i = 0; i < ClipedPoly.size(); ++i)
		ClipedPolyIndices.push_back(i);
	for (size_t i = 0; i < Cliper.size(); ++i)
		CliperIndices.push_back(ClipedPoly.size() + i);

	std::vector<char> bIsIntersectionClipedPoly(ClipedPoly.size(), 0);	//��Ǹõ��Ƿ��ǽ���
	std::vector<char> bIsIntersectionCliper(Cliper.size(), 0);			//��Ǹõ��Ƿ��ǽ���

	//���㽻��
	for (size_t i = 0; i < ClipedPolyIndices.size(); ++i)
	{
		const Vector2D& P00 = AllPoints[ClipedPolyIndices[i]];
		const Vector2D& P01 = (i + 1) < ClipedPolyIndices.size() ? AllPoints[ClipedPolyIndices[i + 1]] : AllPoints[ClipedPolyIndices[0]];
		for (size_t j = 0; j < CliperIndices.size(); ++j)
		{
			const Vector2D& P10 = AllPoints[CliperIndices[j]];
			const Vector2D& P11 = (j + 1) < CliperIndices.size() ? AllPoints[CliperIndices[j + 1]] : AllPoints[CliperIndices[0]];
			if (IsIntersect(P00, P01, P10, P11))
			{
				Vector2D InSec = CalcIntersection(P00, P01, P10, P11);
				AllPoints.push_back(InSec);
				ClipedPolyIndices.insert(ClipedPolyIndices.begin() + i, AllPoints.size() - 1);
				bIsIntersectionClipedPoly.insert(bIsIntersectionClipedPoly.begin() + i, 1);
				CliperIndices.insert(CliperIndices.begin() + j, AllPoints.size() - 1);
				bIsIntersectionCliper.insert(bIsIntersectionCliper.begin() + j, 1);
			}
		}
	}
	if (ClipedPolyIndices.size() == ClipedPoly.size())//˵��û�н���
	{
		if (!IsOnClockwiseSide(ClipedPoly[1] - ClipedPoly[0], Cliper[0] - ClipedPoly[0]))//�ж�Cliper����һ���Ƿ���ClipedPoly�ڲ�,�������˵����������β��ཻ,�ü����Ϊԭ�����
		{
			ResultPolys.push_back(ClipedPoly);
			return;
		}
	}
	//��ʼ�ü�
	std::reverse(CliperIndices.begin(), CliperIndices.end());
	std::reverse(bIsIntersectionCliper.begin(), bIsIntersectionCliper.end());
	std::vector<char> bIsVisited(AllPoints.size(),0);
	std::vector<char> bIsIntersection(bIsIntersectionClipedPoly);
	bIsIntersection.insert(bIsIntersection.end(), bIsIntersectionCliper.begin(), bIsIntersectionCliper.end());
	std::vector<size_t> CombinedIndices(ClipedPolyIndices);
	CombinedIndices.insert(CombinedIndices.end(), CliperIndices.begin(), CliperIndices.end());

	for (size_t i = 0; i < bIsVisited.size();++i)
	{
		if(bIsVisited[i]) continue;
		std::vector<size_t> ClipResult;
		std::vector<Vector2D> ResultEdges;
		if(i < ClipedPolyIndices.size())
			TranvserCliped(bIsVisited, CombinedIndices, bIsIntersection, i, ClipedPolyIndices.size(), CliperIndices.size(), ClipResult);
		else
			TranvserCliper(bIsVisited, CombinedIndices, bIsIntersection, i, ClipedPolyIndices.size(), CliperIndices.size(), ClipResult);
		for (size_t Index : ClipResult) 
			ResultEdges.push_back(AllPoints[Index]);
		if(ClipResult.size() > 0) ResultPolys.push_back(ResultEdges);
		ClipResult.clear();
	}
}
/*
����βü�
@param ClipedPoly		���ü��Ķ����
@param Clipers		�����ü��Ķ����(���ϰ�������)
@return ResultEdges	�ü��������ı�,��˳ʱ��˳��
*/
void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers, std::vector<std::vector<Vector2D>>& ResultPolys)
{
	std::stack<std::vector<Vector2D>> ClipedPolyStack;
	ClipedPolyStack.push(ClipedPoly);

	for (std::vector<Vector2D>& Cliper : Clipers)
	{
		//�Ѳü������µĶ���η�����ü�stack
		for (std::vector<Vector2D>& Poly : ResultPolys)
		{
			ClipedPolyStack.push(Poly);
		}
		ResultPolys.clear();//ÿһ�ֶ�Ҫ���
		while (!ClipedPolyStack.empty())
		{
			std::vector<Vector2D> NewClipedPoly = ClipedPolyStack.top();
			ClipedPolyStack.pop();
			ClipPolygon(NewClipedPoly, Cliper, ResultPolys);
		}
	}
}

struct GridNode
{
	int VisitCount;//��ǰ���ʴ���
	std::vector<size_t> VertexIndices;//�洢��grid�а����Ķ���
};


/*
 ������ζ����������
 @param InputPolys
 @param GridWidth
 @param GridHeight
 @param OutAllVertices
 @param OutAllIndices
*/
void GridizePolys(
	const std::vector<std::vector<Vector2D>>& InputPolys,
	int GridWidth, 
	int GridHeight,
	std::vector<GridNode>& OutGrids,
	std::vector<Vector2D>& OutAllVertices,
	std::vector<Vector2D>& OutRenderVertices,
	std::vector<size_t>& OutAllIndices,
	std::vector<size_t>& OutPolyBelongingTo,
	Bounds2D& OutAllPolysBounds
)
{
	OutGrids.clear();
	OutGrids.resize(GridWidth*GridHeight);
	OutAllVertices.clear();
	OutAllIndices.clear();

	for (size_t PolyIndex = 0; PolyIndex < InputPolys.size(); ++PolyIndex)
	{
		const std::vector<Vector2D>& Poly = InputPolys[PolyIndex];
		OutAllVertices.insert(OutAllVertices.end(), Poly.begin(), Poly.end());
		//if (PolyIndex == 0)
		{
			OutRenderVertices.insert(OutRenderVertices.end(), Poly.begin(), Poly.end());
		}
		//else
		{
			//OutRenderVertices.insert(OutRenderVertices.end(), Poly.rbegin(), Poly.rend());
		}
		size_t StartSize = OutAllIndices.size();
		for (size_t i = 0; i < Poly.size(); ++i)
		{
			OutAllIndices.push_back(StartSize + i);
			OutPolyBelongingTo.push_back(PolyIndex);
			
		}
	}
	//�������ȫ������ε�bounds
	OutAllPolysBounds.Reset();
	for (const Vector2D& V : OutAllVertices)
	{
		OutAllPolysBounds += V;
	}
	OutAllPolysBounds.Min -= Vector2D(1.f, 1.f);
	OutAllPolysBounds.Max += Vector2D(1.f, 1.f);
	float BoundsWidth = OutAllPolysBounds.Max.X - OutAllPolysBounds.Min.X;
	float BoundsHeight = OutAllPolysBounds.Max.Y - OutAllPolysBounds.Min.Y;
	for (size_t Index = 0; Index < OutAllVertices.size(); ++Index)
	{
		const Vector2D& V = OutAllVertices[Index];
		//����õ������ĸ���
		int x = (int)std::floorf(GridWidth * (V.X - OutAllPolysBounds.Min.X)/ BoundsWidth);
		int y = (int)std::floorf(GridHeight * (V.Y - OutAllPolysBounds.Min.Y)/ BoundsHeight);
		GridNode& Node = OutGrids[y*GridWidth + x];
		Node.VertexIndices.push_back(Index);
	}

}

struct DelaunayEdge
{
	size_t VertexIndices[2];
	int AccessID;

	bool operator==(const DelaunayEdge& rhs) const
	{
		return (VertexIndices[0] == rhs.VertexIndices[0] && VertexIndices[1] == rhs.VertexIndices[1])
			|| (VertexIndices[0] == rhs.VertexIndices[1] && VertexIndices[1] == rhs.VertexIndices[0]);
	}

	bool operator!=(const DelaunayEdge& rhs)
	{
		return !this->operator==(rhs);
	}
};

/*
 �ж�P3�Ƿ��P1P2�ɼ�,���ж�P3�Ƿ���P1P2�ұ�,�Ҳ����κζ�����ཻ
 @param InputPolys�����Լ����,
*/
bool IsVisibleTo(const Vector2D& P1, const Vector2D& P2, const Vector2D& P3, const std::vector<std::vector<Vector2D>>& InputPolys)
{
	if (IsOnClockwiseSide(P2 - P1, P3 - P1))
	{
		for (size_t PolyIndex = 0; PolyIndex < InputPolys.size();++PolyIndex)
		{
			const std::vector<Vector2D>& Poly = InputPolys[PolyIndex];
			for (size_t i = 0; i < Poly.size(); ++i)
			{
				const Vector2D& V0 = Poly[i];
				const Vector2D& V1 = (i+1 < Poly.size()) ? Poly[i+1] : Poly[0];
				//if(P1 == V0 || P1 == V1 || P2 == V0 || P2 == V1 || P3 == V0 || P3 == V1) continue;
				if (IsIntersect(P1, P3, V0, V1) || IsIntersect(P2, P3, V0, V1))
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

bool IsTriangleExist(std::vector<std::vector<size_t>>& AllConstructedTriangles, size_t P1, size_t P2, size_t P3)
{
	 for(const std::vector<size_t>& Poly : AllConstructedTriangles)
	 {
		 auto it1 = std::find(Poly.begin(), Poly.end(), P1);
		 auto it2 = std::find(Poly.begin(), Poly.end(), P2);
		 auto it3 = std::find(Poly.begin(), Poly.end(), P3);
		 if ((it1 != Poly.end()) && (it2 != Poly.end())  && (it3 != Poly.end())) return true;
	 }
	 return false;
}

void ConstructDelaunayTriangle(
	const DelaunayEdge& InputEdge,
	const std::vector<GridNode>& InputGrids,
	const std::vector<Vector2D>& InputAllVertices,
	std::vector<std::vector<Vector2D>>& AllPolys,
	const std::vector<size_t>& PolyBelongingTo,
	const Bounds2D& InputAllPolysBounds,
	std::vector<std::vector<size_t>>& AllConstructedTriangles,
	int GridWidth, int GridHeight,
	const std::vector<DelaunayEdge>& InOutAllEdges,
	std::vector<size_t>& InOutAllEdgeIndices,
	std::vector<DelaunayEdge>& InOutEdgeQueue,
	std::vector<size_t>& OutTriangle
	)
{
	const Vector2D& P1 = InputAllVertices[InputEdge.VertexIndices[0]];
	const Vector2D& P2 = InputAllVertices[InputEdge.VertexIndices[1]];

	for (size_t Index = 0; Index < InputAllVertices.size(); ++Index)
	{
		if (Index == InputEdge.VertexIndices[0] || Index == InputEdge.VertexIndices[1]) continue;

		const Vector2D& P3 = InputAllVertices[Index];
		if (IsTriangleExist(AllConstructedTriangles, InputEdge.VertexIndices[0], InputEdge.VertexIndices[1], Index)) continue;

		size_t PolyP3BelongTo = PolyBelongingTo[Index];
		size_t P3Index = Index;
		float fAngle132 = CalcAngle(P1, P3, P2);
		if (IsVisibleTo(P1, P2, P3, AllPolys))
		{
			Circle BoundCircle;
			CalcBoundingCircle(P1, P2, P3, BoundCircle);
			Bounds2D BoundsBox = BoundCircle.ToBounds2D();
			Bounds2D Offsets = InputAllPolysBounds.Offset(BoundsBox);
			int StartX = (int)std::floorf( Offsets.Min.X * GridWidth);
			int StartY = (int)std::floorf(Offsets.Min.Y * GridHeight);
			int EndX = (int)std::ceilf(Offsets.Max.X * GridWidth);
			int EndY = (int)std::ceilf(Offsets.Max.Y * GridHeight);
			StartX = Clamp(StartX, 0, GridWidth-1);
			StartY = Clamp(StartY, 0, GridHeight - 1);
			EndX = Clamp(EndX, 0, GridWidth - 1);
			EndY = Clamp(EndY, 0, GridHeight - 1);
			for (int x = StartX; x <= EndX; ++x)
			{
				for (int y = StartY; y <= EndY; ++y)
				{
					int GridIndex = y * GridWidth + x;
					const GridNode& Node = InputGrids[GridIndex];
					for (size_t VertexIndex : Node.VertexIndices)
					{
						if (VertexIndex == Index) continue;
						if (VertexIndex == InputEdge.VertexIndices[0]) continue;
						if (VertexIndex == InputEdge.VertexIndices[1]) continue;

						const Vector2D& P = InputAllVertices[VertexIndex];
						size_t PolyPBelongTo = PolyBelongingTo[Index];
						if (IsVisibleTo(P1, P2, P, AllPolys))
						{
							float NewAngle = CalcAngle(P1, P, P2);
							if (NewAngle < fAngle132)
							{
								fAngle132 = NewAngle;
								P3Index = VertexIndex;
							}
						}
					}
				}
			}

			//if (IsTriangleExist(AllConstructedTriangles, InputEdge.VertexIndices[0], InputEdge.VertexIndices[1], P3Index)) continue;
			//�µı�
			DelaunayEdge NewEdge1;
			NewEdge1.VertexIndices[0] = P3Index;
			NewEdge1.VertexIndices[1] = InputEdge.VertexIndices[1];
// 			NewEdge1.VertexIndices[0] = InputEdge.VertexIndices[1];
// 			NewEdge1.VertexIndices[1] = P3Index;
			DelaunayEdge NewEdge2;
			NewEdge2.VertexIndices[0] = InputEdge.VertexIndices[0];
			NewEdge2.VertexIndices[1] = P3Index;
// 			NewEdge2.VertexIndices[0] = P3Index;
// 			NewEdge2.VertexIndices[1] = InputEdge.VertexIndices[0];

			//��������ɵı߲���Լ����,���Ѿ��ڶ���������ɾ��,������������
			auto it = std::find(InOutAllEdges.begin(), InOutAllEdges.end(), NewEdge1);
			if (it == InOutAllEdges.end())//����Լ����
			{
				auto qit = std::find(InOutEdgeQueue.begin(), InOutEdgeQueue.end(), NewEdge1);
				if (qit != InOutEdgeQueue.end())//�Ѿ��ڶ���������ɾ��
				{
					InOutEdgeQueue.erase(qit);
				}
				else//������������
				{
					InOutEdgeQueue.push_back(NewEdge1);
				}
			}
			it = std::find(InOutAllEdges.begin(), InOutAllEdges.end(), NewEdge2);
			if (it == InOutAllEdges.end())//����Լ����
			{
				auto qit = std::find(InOutEdgeQueue.begin(), InOutEdgeQueue.end(), NewEdge2);
				if (qit != InOutEdgeQueue.end())//�Ѿ��ڶ���������ɾ��
				{
					InOutEdgeQueue.erase(qit);
				}
				else//������������
				{
					InOutEdgeQueue.push_back(NewEdge2);
				}
			}
			//�ҵ�һ��������
			OutTriangle.push_back(InputEdge.VertexIndices[0]);
			OutTriangle.push_back(P3Index);
			OutTriangle.push_back(InputEdge.VertexIndices[1]);
			
			break;
		}
	}
}

void ConstructTriangleMesh(
	const std::vector<std::vector<Vector2D>>& InputPolys, 
	std::vector<Vector2D>& RenderVertices,
	std::vector<size_t>& AllTrianglesIndices
)
{
	//������
	std::vector<std::vector<Vector2D>> InputPolysCopy(InputPolys);
	std::vector<DelaunayEdge> AllEdges;
	std::vector<size_t> AllEdgeIndices;
	size_t VertexIndex = 0;
	size_t VertexStart = 0;
	for (size_t PolyIndex =0; PolyIndex < InputPolys.size(); ++PolyIndex)
	{
		const std::vector<Vector2D>& Poly = InputPolys[PolyIndex];
		for (size_t i = 0; i < Poly.size(); ++i,++VertexIndex)
		{
			DelaunayEdge Edge;
			Edge.VertexIndices[0] = VertexIndex;
			Edge.VertexIndices[1] = (i + 1 < Poly.size()) ? VertexIndex + 1 : VertexStart;
			AllEdges.push_back(Edge);
			AllEdgeIndices.push_back(AllEdgeIndices.size());
		}
		VertexStart = VertexIndex;
	}

	//��������ٷ���
	std::vector<GridNode> Grids;
	std::vector<size_t> AllVertexIndices;
	std::vector<size_t> PolyBelongingTo;
	Bounds2D Bounds;
	int GridWidth = 100, GridHeight = 100;
	std::vector<Vector2D> AllVertices;
	GridizePolys(InputPolys, GridWidth, GridHeight, Grids, AllVertices, RenderVertices, AllVertexIndices, PolyBelongingTo, Bounds);

	//����Delaunay��������
	std::vector<DelaunayEdge> EdgeQueue(AllEdges.begin(), AllEdges.end());
	std::vector<std::vector<size_t>> Triangles;
	int breaknum = 0;
	while (EdgeQueue.size() > 0)
	{
		breaknum++;

		DelaunayEdge Edge = EdgeQueue.back();
		EdgeQueue.pop_back();

		std::vector<size_t> OutTriangleIndices;
		ConstructDelaunayTriangle(
			Edge,
			Grids,
			AllVertices,
			InputPolysCopy,
			PolyBelongingTo,
			Bounds,
			Triangles,
			GridWidth, GridHeight,
			AllEdges,
			AllEdgeIndices,
			EdgeQueue,
			OutTriangleIndices
		);
		if (OutTriangleIndices.size() > 0)
		{
			Triangles.push_back(OutTriangleIndices);
		}

	}

	for (const std::vector<size_t>& Triangle : Triangles)
	{
		AllTrianglesIndices.insert(AllTrianglesIndices.begin(), Triangle.begin(), Triangle.end());
	}
}
