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

struct AStarNode
{
	unsigned int GridIndex;

	int G;
	int H;

	AStarNode* Parent;
};

template <int PoolSize>
class AStarNodePool
{
public:
	AStarNodePool() :PoolHead(0) {}
	AStarNode* Create(unsigned int InGridIndex, int InG, int InH, AStarNode* InParent = NULL) 
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

static AStarNodePool<512*512> NodePool;

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
bool FindPath_AStar(const std::vector<GridType>& InGrids, unsigned int StartGridIndex, unsigned int DestGridIndex, std::vector<Position2D>& OutPath)
{
	if(StartGridIndex == DestGridIndex) return true; 
	const GridType& DestGrid = InGrids[DestGridIndex];

	NodePool.Reset();

	auto NodeCompare = [](const AStarNode* Lhs, const AStarNode* Rhs) { return (Lhs->G + Lhs->H) < (Rhs->G + Rhs->H); };
	std::multiset<AStarNode*, decltype(NodeCompare)> OpenList(NodeCompare);
	std::set<unsigned int> GridIndexOfOpenList;

	OpenList.insert(NodePool.Create(StartGridIndex, 0,0));
	GridIndexOfOpenList.insert(StartGridIndex);

	std::vector<AStarNode*> CloseList;
	std::set<unsigned int> GridIndexOfCloseList;
	AutoEclipse Time("AStar");
	while (OpenList.size() > 0)
	{
		AStarNode* Candidate = *OpenList.begin();

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
			int deltaG = CurrentGrid.Position.X == CandidateGrid.Position.X || CurrentGrid.Position.Y == CandidateGrid.Position.Y ? 10 : 14;
			int H = 10 * (std::abs(DestGrid.Position.X - CurrentGrid.Position.X) + std::abs(DestGrid.Position.Y - CurrentGrid.Position.Y));
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

int main()
{
	PathFinder<8> PF;
	PF.InitGrids(std::function<void(std::vector<SquareGrid>&, unsigned int, unsigned int)>(BuildSquareGrids), PathFinder<8>::PathFindMethodType(FindPath_AStar<PathFinder<8>::GridType>), (unsigned int)512, (unsigned int)512);

	std::vector<Position2D> OutPath;
	PF.FindPath(0,512*512-1, OutPath);
}

