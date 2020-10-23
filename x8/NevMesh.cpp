#include "NevMesh.h"

#include <stack>
//forward declaration
void TranvserCliper(std::vector<char>& bIsVisited, std::vector<size_t>& ClipedPolyIndices, std::vector<size_t>& ReversedCliperIndices, std::vector<char>& bIsIntersectionClipedPoly, std::vector<char>& bReversedIsIntersectionCliper, size_t StartIndex, std::vector<size_t>& ClipResult);

//�������ü��Ķ����
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
//���������ü��Ķ����
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

void ConstructDelaunayTriangle(const Vector2D& P0, const Vector2D& P1)
{

}
