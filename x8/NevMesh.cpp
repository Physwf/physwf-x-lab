#include "NevMesh.h"

#include <stack>
#include <queue>

//forward declaration
void TranvserCliper(std::vector<char>& bIsVisited, std::vector<size_t>& ClipedPolyIndices, std::vector<size_t>& ReversedCliperIndices, std::vector<char>& bIsIntersectionClipedPoly, std::vector<char>& bReversedIsIntersectionCliper, size_t StartIndex, std::vector<size_t>& ClipResult);

//遍历被裁剪的多边形
void TranvserCliped(
	std::vector<char>& bIsVisited, 
	std::vector<size_t>& ClipedPolyIndices, 
	std::vector<size_t>& ReversedCliperIndices, 
	std::vector<char>& bIsIntersectionClipedPoly, 
	std::vector<char>& bReversedIsIntersectionCliper,
	size_t StartIndex, 
	std::vector<size_t>& ClipResult
)
{
	auto it = std::find(ClipedPolyIndices.begin(), ClipedPolyIndices.end(), StartIndex);
	for (; it != ClipedPolyIndices.end(); ++it)
	{
		if (bIsVisited[*it]) break;
		bIsVisited[*it] = true;
		ClipResult.push_back(*it);
		if (bIsIntersectionClipedPoly[*it])//entering intersection
		{
			TranvserCliper(bIsVisited, ClipedPolyIndices, ReversedCliperIndices, bIsIntersectionClipedPoly, bReversedIsIntersectionCliper, *(++it), ClipResult);
			return;
		}
	}
}
//遍历用来裁剪的多边形
void TranvserCliper(
	std::vector<char>& bIsVisited, 
	std::vector<size_t>& ClipedPolyIndices, 
	std::vector<size_t>& ReversedCliperIndices, 
	std::vector<char>& bIsIntersectionClipedPoly,
	std::vector<char>& bReversedIsIntersectionCliper,
	size_t StartIndex, 
	std::vector<size_t>& ClipResult
)
{
	auto it = std::find(ReversedCliperIndices.begin(), ReversedCliperIndices.end(), StartIndex);
	for (; it != ReversedCliperIndices.end(); ++it)
	{
		if (bIsVisited[*it]) break;
		bIsVisited[*it] = true;
		ClipResult.push_back(*it);
		if (bIsIntersectionClipedPoly[*it])//exit intersection
		{
			TranvserCliped(bIsVisited, ClipedPolyIndices, ReversedCliperIndices, bIsIntersectionClipedPoly, bReversedIsIntersectionCliper, *(++it), ClipResult);
			return;
		}
	}
}

void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<Vector2D>& Cliper, std::vector<std::vector<Vector2D>>& ResultPolys)
{
	if (ClipedPoly.size() < 3 || Cliper.size() < 3) return;

	//将所有顶点放一起,提升效率
	std::vector<Vector2D> AllPoints(ClipedPoly);
	AllPoints.insert(AllPoints.end(), Cliper.begin(), Cliper.end());
	//建立索引
	std::vector<size_t> ClipedPolyIndices;
	std::vector<size_t> CliperIndices;
	for (size_t i = 0; i < ClipedPoly.size(); ++i)
		ClipedPolyIndices.push_back(i);
	for (size_t i = 0; i < Cliper.size(); ++i)
		CliperIndices.push_back(ClipedPoly.size() + i);

	std::vector<char> bIsIntersectionClipedPoly(ClipedPoly.size(), 0);	//标记该点是否是交点
	std::vector<char> bIsIntersectionCliper(Cliper.size(), 0);			//标记该点是否是交点

	//计算交点
	for (size_t i = 0; i < ClipedPolyIndices.size(); ++i)
	{
		const Vector2D& P00 = AllPoints[ClipedPolyIndices[i]];
		const Vector2D& P01 = (i + 1) < ClipedPolyIndices.size() ? AllPoints[ClipedPolyIndices[i + 1]] : AllPoints[ClipedPolyIndices[0]];
		for (size_t j = 0; j < CliperIndices.size(); ++j)
		{
			const Vector2D& P10 = AllPoints[CliperIndices[i]];
			const Vector2D& P11 = (j + 1) < CliperIndices.size() ? AllPoints[CliperIndices[i + 1]] : AllPoints[CliperIndices[0]];
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
	if (ClipedPolyIndices.size() == ClipedPoly.size())//说明没有交点
	{
		if (!IsOnClockwiseSide(ClipedPoly[1] - ClipedPoly[0], Cliper[0] - ClipedPoly[0]))//判断Cliper任意一点是否在ClipedPoly内部,如果不在说明两个多边形不相交,裁剪结果为原多边形
		{
			ResultPolys.push_back(ClipedPoly);
			return;
		}
	}
	//开始裁剪
	std::reverse(CliperIndices.begin(), CliperIndices.end());
	std::reverse(bIsIntersectionCliper.begin(), bIsIntersectionCliper.end());
	std::vector<char> bIsVisited(AllPoints.size(),0);
	size_t i = 0;
	while (!bIsVisited[i])
	{
		std::vector<size_t> ClipResult;
		std::vector<Vector2D> ResultEdges;
		if(i < ClipedPolyIndices.size())
			TranvserCliped(bIsVisited, ClipedPolyIndices, CliperIndices, bIsIntersectionClipedPoly, bIsIntersectionCliper, i, ClipResult);
		else
			TranvserCliper(bIsVisited, ClipedPolyIndices, CliperIndices, bIsIntersectionClipedPoly, bIsIntersectionCliper, i, ClipResult);
		for (size_t Index : ClipResult) 
			ResultEdges.push_back(AllPoints[Index]);
		if(ClipResult.size() > 0) ResultPolys.push_back(ResultEdges);
		ClipResult.clear();
		++i;
	}
}

void ClipPolygon(std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers, std::vector<std::vector<Vector2D>>& ResultPolys)
{
	std::stack<std::vector<Vector2D>> ClipedPolyStack;
	ClipedPolyStack.push(ClipedPoly);

	for (std::vector<Vector2D>& Cliper : Clipers)
	{
		//把裁剪过的新的多边形放入带裁剪stack
		for (std::vector<Vector2D>& Poly : ResultPolys)
		{
			ClipedPolyStack.push(Poly);
		}
		ResultPolys.clear();//每一轮都要清空
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
	int VisitCount;//当前访问次数
	std::vector<size_t> VertexIndices;//存储该grid中包含的顶点
};


/*
 将多边形顶点进行网格化
 @param InputPolys
 @param GridWidth
 @param GridHeight
 @param OutAllVertices
 @param OutAllIndices
*/
void GridizePolys(
	const std::vector<std::vector<Vector2D>>& InputPolys, 
	int GridWidth, int GridHeight, 
	std::vector<GridNode>& OutGrids,
	std::vector<Vector2D>& OutAllVertices,
	std::vector<size_t>& OutAllIndices,
	Bounds2D& OutAllPolysBounds
)
{
	OutGrids.clear();
	OutGrids.resize(GridWidth*GridHeight);
	OutAllVertices.clear();
	OutAllIndices.clear();

	for (const std::vector<Vector2D>& Poly : InputPolys)
	{
		OutAllVertices.insert(OutAllVertices.end(), Poly.begin(), Poly.end());
		for (size_t i = 0; i < Poly.size(); ++i)
		{
			OutAllIndices.push_back(OutAllIndices.size() + i);
		}
	}
	//计算包含全部多边形的bounds
	OutAllPolysBounds.Reset();
	for (const Vector2D& V : OutAllVertices)
	{
		OutAllPolysBounds += V;
	}

	float BoundsWidth = OutAllPolysBounds.Max.X - OutAllPolysBounds.Min.X;
	float BoundsHeight = OutAllPolysBounds.Max.Y - OutAllPolysBounds.Min.Y;
	for (size_t Index = 0; Index < OutAllVertices.size(); ++Index)
	{
		const Vector2D& V = OutAllVertices[Index];
		//计算该点所属的格子
		int x = (int)std::floorf(BoundsWidth / (V.X - OutAllPolysBounds.Min.X));
		int y = (int)std::floorf(BoundsHeight / (V.Y - OutAllPolysBounds.Min.Y));
		GridNode& Node = OutGrids[y*GridWidth + x];
		Node.VertexIndices.push_back(Index);
	}

}

struct DelaunayEdge
{
	size_t VertexIndices[2];

	bool operator==(const DelaunayEdge& rhs)
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
 判断P3是否对P1P2可见,即判断P3是否在P1P2右边,且不与任何多边形相交
 @param InputPolys多边形约束边,
*/
bool IsVisibleTo(const Vector2D& P1, const Vector2D& P2, const Vector2D& P3, const std::vector<std::vector<Vector2D>>& InputPolys)
{
	if (IsOnClockwiseSide(P2 - P1, P3 - P1))
	{
		for (const std::vector<Vector2D>& Poly : InputPolys)
		{
			for (size_t i = 0; i < Poly.size(); ++i)
			{
				const Vector2D& V0 = Poly[i];
				const Vector2D& V1 = (i+1 < Poly.size()) ? Poly[i+1] : Poly[0];
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

void ConstructDelaunayTriangle(
	const DelaunayEdge& InputEdge,
	const std::vector<GridNode>& InputGrids,
	const std::vector<Vector2D>& InputAllVertices,
	const std::vector<std::vector<Vector2D>>& InputPolys,
	const std::vector<size_t>& InputAllIndices,
	const Bounds2D& InputAllPolysBounds,
	int GridWidth, int GridHeight,
	std::vector<DelaunayEdge>& InOutAllEdges,
	std::vector<size_t>& InOutAllEdgeIndices,
	std::vector<size_t>& InOutEdgeQueue,
	std::vector<size_t>& OutTriangle
	)
{
	const Vector2D& P1 = InputAllVertices[InputEdge.VertexIndices[0]];
	const Vector2D& P2 = InputAllVertices[InputEdge.VertexIndices[1]];

	for (size_t Index = 0; Index < InputAllVertices.size(); ++Index)
	{
		if (Index == InputEdge.VertexIndices[0] || Index == InputEdge.VertexIndices[1]) continue;

		const Vector2D& P3 = InputAllVertices[Index];
		size_t P3Index = Index;
		float fAngle132 = CalcAngle(P1, P3, P2);
		if (IsVisibleTo(P1, P2, P3, InputPolys))
		{
			Circle BoundCircle;
			CalcBoundingCircle(P1, P2, P3, BoundCircle);
			Bounds2D BoundsBox = BoundCircle.ToBounds2D();
			Bounds2D Offsets = InputAllPolysBounds.Offset(BoundsBox);
			int StartX = (int)std::floorf( Offsets.Min.X * GridWidth);
			int StartY = (int)std::floorf(Offsets.Min.Y * GridHeight);
			int EndX = (int)std::ceilf(Offsets.Max.X * GridWidth);
			int EndY = (int)std::ceilf(Offsets.Max.Y * GridHeight);
			StartX = Clamp(StartX, 0, GridWidth);
			StartY = Clamp(StartY, 0, GridHeight);
			EndX = Clamp(EndX, 0, GridWidth);
			EndY = Clamp(EndY, 0, GridHeight);
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
						if (IsVisibleTo(P1, P2, P, InputPolys))
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
			//找到一个三角形
			OutTriangle.push_back(InputEdge.VertexIndices[0]);
			OutTriangle.push_back(InputEdge.VertexIndices[1]);
			OutTriangle.push_back(P3Index);
			//新的边
			DelaunayEdge NewEdge1;
			NewEdge1.VertexIndices[0] = InputEdge.VertexIndices[1];
			NewEdge1.VertexIndices[1] = P3Index;
			DelaunayEdge NewEdge2;
			NewEdge2.VertexIndices[0] = P3Index;
			NewEdge2.VertexIndices[1] = InputEdge.VertexIndices[0];
			//如果第一次发现这个边,则加入队列
			auto it = std::find(InOutAllEdges.begin(), InOutAllEdges.end(), NewEdge1);
			if (it == InOutAllEdges.end())
			{
				InOutAllEdges.push_back(NewEdge1);
				size_t NewEdgeIndex = InOutAllEdgeIndices.size();
				InOutAllEdgeIndices.push_back(NewEdgeIndex);
				InOutEdgeQueue.push_back(NewEdgeIndex);
			}
			else//否则从队列移除
			{
				size_t EdgeIndex = it - InOutAllEdges.begin();
				auto qit = std::find(InOutEdgeQueue.begin(), InOutEdgeQueue.end(), EdgeIndex);
				InOutEdgeQueue.erase(qit);
			}
			it = std::find(InOutAllEdges.begin(), InOutAllEdges.end(), NewEdge2);
			if (it == InOutAllEdges.end())
			{
				InOutAllEdges.push_back(NewEdge2);
				size_t NewEdgeIndex = InOutAllEdgeIndices.size();
				InOutAllEdgeIndices.push_back(NewEdgeIndex);
				InOutEdgeQueue.push_back(NewEdgeIndex);
			}
			else//否则从队列移除
			{
				size_t EdgeIndex = it - InOutAllEdges.begin();
				auto qit = std::find(InOutEdgeQueue.begin(), InOutEdgeQueue.end(), EdgeIndex);
				InOutEdgeQueue.erase(qit);
			}
		}
	}
}

void ConstructTriangleMesh(
	const std::vector<std::vector<Vector2D>>& InputPolys, 
	std::vector<std::vector<Vector2D>>& OutTriangles
)
{
	//构建边
	std::vector<DelaunayEdge> AllEdges;
	std::vector<size_t> AllEdgeIndices;
	size_t VertexIndex = 0;
	for (const std::vector<Vector2D>& Poly : InputPolys)
	{
		for (size_t i = 0; i < Poly.size(); ++i,++VertexIndex)
		{
			DelaunayEdge Edge;
			Edge.VertexIndices[0] = VertexIndex; Poly[i];
			Edge.VertexIndices[1] = (i+1 < Poly.size()) ? VertexIndex + 1 : 0;
			AllEdges.push_back(Edge);
			AllEdgeIndices.push_back(AllEdgeIndices.size());
		}
	}

	//用网格加速访问
	std::vector<GridNode> Grids;
	std::vector<Vector2D> AllVertices;
	std::vector<size_t> AllIndices;
	Bounds2D Bounds;
	int GridWidth = 100, GridHeight = 100;
	GridizePolys(InputPolys, GridWidth, GridHeight, Grids, AllVertices, AllIndices, Bounds);

	//生成Delaunay三角网格
	std::vector<size_t> EdgeQueue(AllEdgeIndices.begin(),AllEdgeIndices.end());
	std::vector<std::vector<size_t>> Triangles;
	while (EdgeQueue.size() > 0)
	{
		size_t Index = EdgeQueue.back();
		EdgeQueue.pop_back();

		const DelaunayEdge& Edge = AllEdges[Index];
		std::vector<size_t> OutTriangleIndices;
		ConstructDelaunayTriangle(
			Edge,
			Grids,
			AllVertices,
			InputPolys,
			AllIndices,
			Bounds,
			GridWidth, GridHeight,
			AllEdges,
			AllEdgeIndices,
			EdgeQueue,
			OutTriangleIndices
		);
		Triangles.push_back(OutTriangleIndices);
	}

	//从索引转换成坐标
	OutTriangles.resize(Triangles.size());
	for (std::vector<size_t>& Indices : Triangles)
	{
		std::vector<Vector2D> Vertices(3);
		for (int i = 0; i < 3; ++i)
		{
			Vertices[i] = AllVertices[Indices[i]];
		}
	}
}
