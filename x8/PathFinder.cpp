#include "PathFinder.h"
#include <set>
#include <stdio.h>
#include <cassert>

#include "Stats.h"

typedef Grid<8> SquareGrid;

void BuildSquareGrids(std::vector<SquareGrid>& OutGrids, unsigned int NumberGridsInWidth, unsigned int NumberGridsInHeight)
{
	OutGrids.resize(NumberGridsInWidth*NumberGridsInHeight);
	for (unsigned int y = 0; y < NumberGridsInHeight; ++y)
	{
		for (unsigned int x = 0; x < NumberGridsInWidth; ++x)
		{
			const unsigned int CurrentIndex = y * NumberGridsInWidth + x;
			SquareGrid& SG = OutGrids[CurrentIndex];
			SG.Position = { (int)x,(int)y };
			//四个格子,左上右下
			if (x > 0)															SG.AddNeighbor(CurrentIndex - 1);
			if (y > 0)															SG.AddNeighbor(CurrentIndex - NumberGridsInWidth);
			if (x < NumberGridsInWidth - 1)										SG.AddNeighbor(CurrentIndex + 1);
			if (y < NumberGridsInHeight - 1)									SG.AddNeighbor(CurrentIndex + NumberGridsInWidth);
			if (x > 0 && y > 0)													SG.AddNeighbor(CurrentIndex - NumberGridsInWidth - 1);//左上
			if (x < NumberGridsInWidth - 1 && y > 0)							SG.AddNeighbor(CurrentIndex - NumberGridsInWidth + 1);//右上
			if (x < NumberGridsInWidth - 1 && y < NumberGridsInHeight - 1)		SG.AddNeighbor(CurrentIndex + NumberGridsInWidth + 1);//右下
			if (x > 0 && y < NumberGridsInHeight - 1)							SG.AddNeighbor(CurrentIndex + NumberGridsInWidth - 1);//左下
		}
	}
}

int SquareGridGValueCalculator(const GridPoint& A, const GridPoint& B)
{
	return A.X == B.X || A.Y == B.Y ? 10 : 14;
}

int SquareGridHValueCalculator(const GridPoint& Current, const GridPoint& Dest)
{
	return 10 * (std::abs(Dest.X - Current.X) + std::abs(Dest.Y - Current.Y));
}

struct AStarNode
{
	GridIDType GridIndex;

	int G;
	int H;

	AStarNode* Parent;
};

template <int PoolSize>
class AStarNodePool
{
public:
	AStarNodePool() :PoolHead(0) {}
	AStarNode* Create(GridIDType InGridIndex, int InG, int InH, AStarNode* InParent = NULL)
	{ 
		assert(PoolHead + 1 < PoolSize);;
		NodePool[PoolHead] = { InGridIndex, InG, InH, InParent };
		return &NodePool[PoolHead++];
	}
	void Reset()
	{
		PoolHead = 0;
	}
private:
	AStarNode NodePool[PoolSize];
	unsigned int PoolHead;
};

static AStarNodePool<100000> NodePool;

template <typename GridType> 
void BuildPathFromNode(const std::vector<GridType>& InGrids, std::vector<Position2D>& OutPath, AStarNode* DestNode)
{
	while (DestNode->Parent)
	{
		const GridType& Grid = InGrids[DestNode->GridIndex];
		OutPath.push_back({ (float)Grid.Position.X, (float)Grid.Position.Y });
		DestNode = DestNode->Parent;
	}
}

template <typename GridType>
bool FindPath_AStar(const std::vector<GridType>& InGrids, GridIDType StartGridIndex, GridIDType DestGridIndex, std::vector<Position2D>& OutPath, GValueCalculator GValueFunc, HValueCalculator HValueFunc)
{
	if(StartGridIndex == DestGridIndex) return true; 
	const GridType& DestGrid = InGrids[DestGridIndex];

	NodePool.Reset();

	auto NodeCompare = [](const AStarNode* Lhs, const AStarNode* Rhs) { return (Lhs->G + Lhs->H) < (Rhs->G + Rhs->H); };
	std::multiset<AStarNode*, decltype(NodeCompare)> OpenList(NodeCompare);
	std::set<GridIDType> GridIndexOfOpenList;

	OpenList.insert(NodePool.Create(StartGridIndex, 0,0));
	GridIndexOfOpenList.insert(StartGridIndex);

	std::vector<AStarNode*> CloseList;
	std::set<GridIDType> GridIndexOfCloseList;

	AutoEclipse Time("AStar");
	int NumNodeVisited = 0;
	AutoCall AC([&]() { printf("NumNodeVisited:%d\n", NumNodeVisited); });
	while (OpenList.size() > 0)
	{
		AStarNode* Candidate = *OpenList.begin();
		++NumNodeVisited;
		CloseList.push_back(Candidate);
		GridIndexOfCloseList.insert(Candidate->GridIndex);
		OpenList.erase(OpenList.begin());
		GridIndexOfOpenList.erase(Candidate->GridIndex);
		const GridType& CandidateGrid = InGrids[Candidate->GridIndex];
		//printf("candidate x=%d,y=%d\n", CandidateGrid.Position.X, CandidateGrid.Position.Y);
		//printf("--------\n");
		for (unsigned int Index = 0; Index < CandidateGrid.NumNeighbors; ++Index)
		{
			unsigned int NeighborGridIndex = CandidateGrid.Neighbors[Index];
			if (GridIndexOfOpenList.find(NeighborGridIndex) != GridIndexOfOpenList.end() || GridIndexOfCloseList.find(NeighborGridIndex) != GridIndexOfCloseList.end())
			{
				continue;
			}
			const GridType& CurrentGrid = InGrids[NeighborGridIndex];
			//printf("x=%d,y=%d\n", CurrentGrid.Position.X, CurrentGrid.Position.Y);
			int deltaG = GValueFunc(CurrentGrid.Position, CandidateGrid.Position); 
			int H = HValueFunc(CurrentGrid.Position,DestGrid.Position);
			AStarNode* CurrentNode = NodePool.Create(NeighborGridIndex, Candidate->G + deltaG, H, Candidate);
			if (NeighborGridIndex == DestGridIndex)
			{
				BuildPathFromNode(InGrids, OutPath, CurrentNode);
				return true;
			}
			GridIndexOfOpenList.insert(NeighborGridIndex);
			OpenList.insert(CurrentNode);
		}
		//printf("--------\n");
	}
	return false;

}

int _main()
{
	{
		unsigned int Size = 512;
		PathFinder<8> PF;
		PF.InitGrids(std::function<void(std::vector<SquareGrid>&, GridIDType, GridIDType)>(BuildSquareGrids), 
			PathFinder<8>::PathFindMethodType(FindPath_AStar<PathFinder<8>::GridType>), 
			SquareGridGValueCalculator,
			SquareGridHValueCalculator,
			(GridIDType)512, (GridIDType)512);
		std::vector<Position2D> OutPath;
		PF.FindPath(0, 512 * 512 - 1, OutPath);
	}

	{
		unsigned int Size = 1024;
		PathFinder<8> PF;
		PF.InitGrids(std::function<void(std::vector<SquareGrid>&, GridIDType, GridIDType)>(BuildSquareGrids),
			PathFinder<8>::PathFindMethodType(FindPath_AStar<PathFinder<8>::GridType>),
			SquareGridGValueCalculator,
			SquareGridHValueCalculator,
			(GridIDType)Size, (GridIDType)Size);
		std::vector<Position2D> OutPath;
		PF.FindPath(0, Size * Size - 1, OutPath);
	}

	{
		unsigned int Size = 2048;
		PathFinder<8> PF;
		PF.InitGrids(std::function<void(std::vector<SquareGrid>&, GridIDType, GridIDType)>(BuildSquareGrids),
			PathFinder<8>::PathFindMethodType(FindPath_AStar<PathFinder<8>::GridType>),
			SquareGridGValueCalculator,
			SquareGridHValueCalculator,
			(GridIDType)Size, (GridIDType)Size);
		std::vector<Position2D> OutPath;
		PF.FindPath(0, Size * Size - 1, OutPath);
	}

	{
		unsigned int Size = 4096;
		PathFinder<8> PF;
		PF.InitGrids(std::function<void(std::vector<SquareGrid>&, GridIDType, GridIDType)>(BuildSquareGrids),
			PathFinder<8>::PathFindMethodType(FindPath_AStar<PathFinder<8>::GridType>),
			SquareGridGValueCalculator,
			SquareGridHValueCalculator,
			(GridIDType)Size, (GridIDType)Size);
		std::vector<Position2D> OutPath;
		PF.FindPath(0, Size * Size - 1, OutPath);
	}
	return 0;
}

