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

typedef unsigned int GridIDType;

template<unsigned char MaxNeighborNumber>
struct Grid
{
	GridPoint Position;
	//EBlockType BlockType;
	//œ‡¡⁄Õ¯∏Ò
	GridIDType Neighbors[MaxNeighborNumber];
	unsigned char NumNeighbors;

	Grid():NumNeighbors(0) {}

	void AddNeighbor(GridIDType NeighborIndex)
	{
		Neighbors[NumNeighbors++] = NeighborIndex;
	}


};

typedef std::function<int(const GridPoint& A, const GridPoint& B)>				GValueCalculator;
typedef std::function<int(const GridPoint& Current, const GridPoint& Dest)>		HValueCalculator;

struct Position2D
{
	float X, Y;
};

template <unsigned char NumberMaxNeighbor>
class PathFinder
{
public:
	typedef Grid<NumberMaxNeighbor> GridType;
	typedef std::function<bool(const std::vector<GridType>&, GridIDType StartGridIndex, GridIDType DestGridIndex, std::vector<Position2D>&, GValueCalculator, HValueCalculator)> PathFindMethodType;

	template <typename... ArgTypes>
	void InitGrids(std::function<void(std::vector<GridType>&, ArgTypes...)> GridsConstructor, PathFindMethodType InPathFindMethod, GValueCalculator GValueFunc, HValueCalculator HValueFunc, ArgTypes... Args)
	{
		GridsConstructor(Grids, std::forward<ArgTypes>(Args)...);
		PathFindMethod = InPathFindMethod;
		GValueCaculateFunc = GValueFunc;
		HValueCaculateFunc = HValueFunc;
	}

	bool FindPath(GridIDType StartGridIndex, GridIDType DestGridIndex, std::vector<Position2D>& OutPath)
	{
		return PathFindMethod(Grids, StartGridIndex, DestGridIndex, OutPath, GValueCaculateFunc, HValueCaculateFunc);
	}

private:
	std::vector<GridType> Grids;
	PathFindMethodType PathFindMethod;
	GValueCalculator GValueCaculateFunc;
	HValueCalculator HValueCaculateFunc;
};