#pragma once

#include <vector>
#include <functional>
#include <utility>

struct GridPoint
{
	int X, Y;
};

enum class EBlockType
{
	Walkable,
	Block,
};

template<unsigned char MaxNeighborNumber>
struct Grid
{
	GridPoint Position;
	EBlockType BlockType;
	//œ‡¡⁄Õ¯∏Ò
	unsigned int Neighbors[MaxNeighborNumber];
	unsigned char NumNeighbors;

	Grid():NumNeighbors(0) {}

	void AddNeighbor(unsigned int NeighborIndex)
	{
		Neighbors[NumNeighbors++] = NeighborIndex;
	}
};

struct Position2D
{
	float X, Y;
};

template <unsigned char NumberMaxNeighbor>
class PathFinder
{
public:

	typedef Grid<NumberMaxNeighbor> GridType;
	typedef std::function<bool(const std::vector<GridType>&, unsigned int StartGridIndex, unsigned int DestGridIndex, std::vector<Position2D>&)> PathFindMethodType;

	template <typename... ArgTypes>
	void InitGrids(std::function<void(std::vector<GridType>&, ArgTypes...)> GridsConstructor, PathFindMethodType InPathFindMethod, ArgTypes... Args)
	{
		GridsConstructor(Grids, std::forward<ArgTypes>(Args)...);
		PathFindMethod = InPathFindMethod;
	}

	bool FindPath(unsigned int StartGridIndex, unsigned int DestGridIndex, std::vector<Position2D>& OutPath)
	{
		return PathFindMethod(Grids, StartGridIndex, DestGridIndex, OutPath);
	}

private:
	std::vector<GridType> Grids;
	PathFindMethodType PathFindMethod;
};