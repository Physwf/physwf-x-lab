#include "bvh.h"
#include "interaction.h"
#include "paramset.h"
#include "parallel.h"
#include <algorithm>

struct BVHPrimitiveInfo
{
	BVHPrimitiveInfo() {}
	BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f& bounds)
		:primitiveNumber(primitiveNumber), bounds(bounds),
		centroid(0.5f*bounds.pMin + 0.5f*bounds.pMax) {}
	size_t primitiveNumber;
	Bounds3f bounds;
	Point3f centroid;
};

struct BVHBuildNode
{
	void InitLeaf(int first, int n, const Bounds3f& b)
	{
		firstPrimOffset = first;
		nPrimitives = n;
		bounds = b;
		children[0] = children[1] = nullptr;
	}
	void InitInterior(int axis, BVHBuildNode* c0, BVHBuildNode* c1)
	{
		children[0] = c0;
		children[1] = c1;
		bounds = Union(c0->bounds, c1->bounds);
		splitAxis = axis;
		nPrimitives = 0;
	}
	Bounds3f bounds;
	BVHBuildNode* children[2];
	int splitAxis, firstPrimOffset, nPrimitives;
};

struct MortonPrimitive
{
	int primitiveIndex;
	uint32_t mortonCode;
};

struct LinearBVHNode
{
	Bounds3f bounds;
	union
	{
		int primitivesOffset;
		int secondChildOffset;
	};
	uint16_t nPrimitives;
	uint8_t axis;
	uint8_t pad[1];
};
struct LBVHTreelet
{
	int startIndex, nPrimitives;
	BVHBuildNode* buildNodes;
};

inline uint32_t LeftShift3(uint32_t x)
{
	DOCHECK(x <= (1 << 10));
	if (x == (1 << 10)) --x;
	x = (x | (x << 16)) & 0b00000011000000000000000011111111;
	// x = ---- --98 ---- ---- ---- ---- 7654 3210
	x = (x | (x << 8)) & 0b00000011000000001111000000001111;
	// x = ---- --98 ---- ---- 7654 ---- ---- 3210
	x = (x | (x << 4)) & 0b00000011000011000011000011000011;
	// x = ---- --98 ---- 76-- --54 ---- 32-- --10
	x = (x | (x << 2)) & 0b00001001001001001001001001001001;
	// x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
	return x;
}

inline uint32_t EncodeMorton3(const Vector3f &v)
{
	DOCHECK(v.x >= 0);
	DOCHECK(v.y >= 0);
	DOCHECK(v.z >= 0);
	return (LeftShift3(v.z) << 2) | (LeftShift3(v.y) << 1) | LeftShift3(v.x);
}
//��������:https://baike.baidu.com/item/��������/7875498
static void RadixSort(std::vector<MortonPrimitive> *v)
{
	std::vector<MortonPrimitive> tempVector(v->size());
	constexpr int bitsPerPass = 6;
	constexpr int nBits = 30;
	constexpr int nPasses = nBits / bitsPerPass;
	for (int pass =0;pass < nPasses;++pass)
	{
		int lowBit = pass * bitsPerPass;
		std::vector<MortonPrimitive> &in = (pass & 1) ? tempVector : *v;
		std::vector<MortonPrimitive> &out = (pass & 1) ? *v : tempVector;
		constexpr int nBuckets = 1 << bitsPerPass;
		int bucketCount[nBuckets] = { 0 };
		constexpr int bitMask = (1 << bitsPerPass) - 1;
		for (const MortonPrimitive& mp : in)
		{
			int bucket = (mp.mortonCode >> lowBit) & bitMask;
			++bucketCount[bucket];
		}
		int outIndex[nBuckets];
		outIndex[0] = 0;
		for (int i = 1; i < nBuckets; ++i)
		{
			outIndex[i] = outIndex[i - 1] + bucketCount[i - 1];
		}
		for (const MortonPrimitive& mp : in)
		{
			int bucket = (mp.mortonCode >> lowBit) & bitMask;
			out[outIndex[bucket]++] = mp;
		}
		if (nPasses & 1)
			std::swap(*v, tempVector);
	}
}

BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive>> &p, int maxPrimsInNode,SplitMethod splitMethod) 
	: maxPrimsInNode(std::min(255, maxPrimsInNode)),
		primitives(p),
		splitMethod(splitMethod)
{
	if (primitives.size() == 0)
		return;

	std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
	for (size_t i = 0; i < primitives.size(); ++i)
	{
		primitiveInfo[i] = { i,primitives[i]->WorldBound() };
	}

	MemoryArena arena(1024 * 1024);
	int totalNodes = 0;
	std::vector<std::shared_ptr<Primitive>> orderedPrims;
	BVHBuildNode* root;
	if (splitMethod == SplitMethod::HLBVH)
	{
		root = HLBVHBuild(arena,primitiveInfo,&totalNodes,orderedPrims);
	}
	else
	{
		root = recursiveBuild(arena, primitiveInfo, 0, primitives.size(), &totalNodes, orderedPrims);
	}
	primitives.swap(orderedPrims);
	primitiveInfo.resize(0);

	nodes = AllocAligned<LinearBVHNode>(totalNodes);
	int offset = 0;
	flattenBVHTree(root,&offset);
}

Bounds3f BVHAccel::WorldBound() const
{
	return nodes ? nodes[0].bounds : Bounds3f();
}

BVHAccel::~BVHAccel()
{
	FreeAligned(nodes);
}

bool BVHAccel::Intersect(const Ray &ray, SurfaceInteraction *isect) const
{
	struct Clock
	{
		Clock()
		{
			startTime = std::chrono::system_clock::now();
		}
		~Clock()
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			int64_t elapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
			if (elapsedMS > 0) printf("elapsedMS:%lld\n", elapsedMS);
		}
		std::chrono::system_clock::time_point startTime;
	};

	bool hit = false;
	Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
	int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
	int toVisitOffset = 0, currentNodeIndex = 0;
	int nodesToVisit[64];
	static int i = 0;
	while (true)
	{
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (node->bounds.IntersectP(ray, invDir, dirIsNeg))
		{

			if (node->nPrimitives > 0)//leaf
			{

				for (int i = 0; i < node->nPrimitives; ++i)
				{
					//Clock c;

					if (primitives[node->primitivesOffset + i]->Intersect(ray, isect))
						hit = true;
				}
				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else//interior node 
			{
				if (dirIsNeg[node->axis])
				{
					nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
					currentNodeIndex = node->secondChildOffset;
				}
				else
				{
					nodesToVisit[toVisitOffset++] = node->secondChildOffset;
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else
		{
			if(toVisitOffset == 0) break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
// 	if (hit)
// 	{
// 		i++;
// 		printf("i=%d\n",i);
// 	}
	return hit;
}

bool BVHAccel::IntersectP(const Ray &ray) const
{
	Vector3f invDir(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);
	int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
	int nodesToVisit[64];
	int toVisitOffset = 0, currentNodeIndex = 0;
	while (true) {
		const LinearBVHNode *node = &nodes[currentNodeIndex];
		if (node->bounds.IntersectP(ray, invDir, dirIsNeg)) {
			// Process BVH node _node_ for traversal
			if (node->nPrimitives > 0) {
				for (int i = 0; i < node->nPrimitives; ++i) {
					if (primitives[node->primitivesOffset + i]->IntersectP(
						ray)) {
						return true;
					}
				}
				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				if (dirIsNeg[node->axis]) {
					/// second child first
					nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
					currentNodeIndex = node->secondChildOffset;
				}
				else {
					nodesToVisit[toVisitOffset++] = node->secondChildOffset;
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else {
			if (toVisitOffset == 0) break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return false;
}

BVHBuildNode* BVHAccel::HLBVHBuild(MemoryArena& arena, 
	const std::vector<BVHPrimitiveInfo>& primitiveInfo, 
	int* totalNodes, 
	std::vector<std::shared_ptr<Primitive>>& orderedPrims) const
{
	//�������ĵ㹹�ɵ�bound
	Bounds3f bounds;
	for (const BVHPrimitiveInfo& pi : primitiveInfo)
	{
		bounds = Union(bounds, pi.centroid);
	}
	//����ÿ��primitive��morton code
	std::vector<MortonPrimitive> mortonPrims(primitiveInfo.size());
	ParallelFor([&](int64_t i) 
		{
			constexpr int mortonBits = 10;
			constexpr int mortonScale = 1 << mortonBits;
			mortonPrims[i].primitiveIndex = primitiveInfo[i].primitiveNumber;
			Vector3f centeroidOffset = bounds.Offset(primitiveInfo[i].centroid);
			mortonPrims[i].mortonCode = EncodeMorton3(centeroidOffset * mortonScale);
		}, primitiveInfo.size(), 512);
	//��������
	RadixSort(&mortonPrims);
	//������primitive���ռ仮�ֳ�2^12(4096)��grid,ÿ��ά����2^4��16����
	std::vector<LBVHTreelet> treeletsToBuild;
	for (int start = 0, end = 1; end <= (int)mortonPrims.size(); ++end)
	{
		uint32_t mask = 0b00111111111111000000000000000000;
		if (end == (int)mortonPrims.size() || ((mortonPrims[start].mortonCode & mask) != (mortonPrims[end].mortonCode & mask)))
		{
			int nPrimitives = end - start;
			int maxBVHNodes = 2 * nPrimitives;
			BVHBuildNode* nodes = arena.Alloc<BVHBuildNode>(maxBVHNodes, false);
			treeletsToBuild.push_back({ start,nPrimitives,nodes });
			start = end;
		}
	}
	//����emitLBVH��ÿ��grid����LBVH
	std::atomic<int> atomicTotal(0), orderedPrimsOffset(0);
	orderedPrims.resize(primitives.size());
	ParallelFor([&](int64_t i)
		{
			int nodesCreated = 0;
			const int firstBitIndex = 29 - 12;
			LBVHTreelet& tr = treeletsToBuild[i];
			tr.buildNodes = emitLBVH(tr.buildNodes,primitiveInfo,&mortonPrims[tr.startIndex],tr.nPrimitives,&nodesCreated,orderedPrims,&orderedPrimsOffset,firstBitIndex);
			atomicTotal += nodesCreated;
		}, treeletsToBuild.size());
	*totalNodes = atomicTotal;
	//����surface area huritic�����ϲ�Ľڵ�(��������4096��grid)
	std::vector<BVHBuildNode*> finishedTreelets;
	finishedTreelets.reserve(treeletsToBuild.size());
	for (LBVHTreelet& treelet : treeletsToBuild)
		finishedTreelets.push_back(treelet.buildNodes);

	return buildUpperSAH(arena,finishedTreelets,0,finishedTreelets.size(),totalNodes);
}

BVHBuildNode* BVHAccel::emitLBVH(BVHBuildNode*& buildNodes, 
	const std::vector<BVHPrimitiveInfo>& primitiveInfo, 
	MortonPrimitive* mortonPrims, 
	int nPrimitives, 
	int* totalNodes, 
	std::vector<std::shared_ptr<Primitive>> &orderedPrims, 
	std::atomic<int> *orderedPrimsOffset, int bitIndex) const
{
	//����Ҷ�ڵ�
	if (bitIndex == -1 || nPrimitives < maxPrimsInNode)
	{
		(*totalNodes)++;
		BVHBuildNode* node = buildNodes++;
		Bounds3f bounds;
		int firstPrimOffset = orderedPrimsOffset->fetch_add(nPrimitives);
		for (int i = 0; i < nPrimitives; ++i)
		{
			int primitiveIndex = mortonPrims[i].primitiveIndex;
			orderedPrims[firstPrimOffset + i] = primitives[primitiveIndex];
			bounds = Union(bounds, primitiveInfo[primitiveIndex].bounds);
		}
		node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
		return node;
	}
	else
	{
		int mask = 1 << bitIndex;
		//�����һ�������һ��primitive��Morton code��ͬ,��ζ��������slit paln��ͬһ��, ��ʱֱ�ӽ����¸�level��һ��ϸ��
		if ((mortonPrims[0].mortonCode & mask) == (mortonPrims[nPrimitives - 1].mortonCode & mask))
		{
			return emitLBVH(buildNodes, primitiveInfo, mortonPrims, nPrimitives, totalNodes, orderedPrims, orderedPrimsOffset, bitIndex - 1);
		}
		//���primitive�ֲ���slit plan����,���ҵ��ָ��,morton code�ı仯��
		int searchStart = 0, searchEnd = nPrimitives - 1;
		while (searchStart + 1 != searchEnd)
		{
			DOCHECK(searchStart != searchEnd);
			int mid = (searchStart + searchEnd) / 2;
			if ((mortonPrims[searchStart].mortonCode & mask) == (mortonPrims[mid].mortonCode & mask))
			{
				searchStart = mid;
			}
			else
			{
				DOCHECK((mortonPrims[mid].mortonCode & mask) == (mortonPrims[searchEnd].mortonCode & mask));
				searchEnd = mid;
			}
		}

		int splitOffset = searchEnd;
		DOCHECK(splitOffset <= nPrimitives - 1);
		DOCHECK((mortonPrims[splitOffset - 1].mortonCode & mask) != (mortonPrims[splitOffset].mortonCode & mask));
		(*totalNodes)++;
		//�����м�ڵ�,�������ָ�
		BVHBuildNode* node = buildNodes++;
		BVHBuildNode* lbvh[2] = {
			emitLBVH(buildNodes,primitiveInfo,mortonPrims,splitOffset,totalNodes,orderedPrims,orderedPrimsOffset,bitIndex-1),
			emitLBVH(buildNodes,primitiveInfo,&mortonPrims[splitOffset],nPrimitives - splitOffset, totalNodes, orderedPrims, orderedPrimsOffset, bitIndex - 1)
		};
		int axis = bitIndex % 3;
		node->InitInterior(axis, lbvh[0], lbvh[1]);
		return node;
	}
}

BVHBuildNode* BVHAccel::buildUpperSAH(MemoryArena& arena, 
	std::vector<BVHBuildNode*> &treeletRoots, 
	int start, int end, 
	int* totalNodes) const
{
	int nNodes = end - start;
	if (nNodes == 1) return treeletRoots[start];
	(*totalNodes)++;

	BVHBuildNode* node = arena.Alloc<BVHBuildNode>();
	Bounds3f bounds;
	for (int i = start; i < end; ++i)
	{
		bounds = Union(bounds, treeletRoots[i]->bounds);
	}

	Bounds3f centroidBounds;
	for (int i = start; i < end; ++i)
	{
		Point3f centorid = (treeletRoots[i]->bounds.pMin + treeletRoots[i]->bounds.pMax) * 0.5f;
		centroidBounds = Union(centroidBounds, centorid);
	}
	int dim = centroidBounds.MaximumExtent();
	DOCHECK(centroidBounds.pMax[dim] != centroidBounds.pMin[dim]);

	constexpr int nBuckets = 12;
	struct BucketInfo
	{
		int count = 0;
		Bounds3f bounds;
	};
	BucketInfo buckets[nBuckets];
	// Initialize _BucketInfo_ for HLBVH SAH partition buckets
	for (int i = start; i < end; ++i) {
		Float centroid = (treeletRoots[i]->bounds.pMin[dim] + treeletRoots[i]->bounds.pMax[dim]) * 0.5f;
		int b = nBuckets * ((centroid - centroidBounds.pMin[dim]) / (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]));
		if (b == nBuckets) b = nBuckets - 1;
		//CHECK_GE(b, 0);
		//CHECK_LT(b, nBuckets);
		buckets[b].count++;
		buckets[b].bounds = Union(buckets[b].bounds, treeletRoots[i]->bounds);
	}

	// Compute costs for splitting after each bucket
	Float cost[nBuckets - 1];
	for (int i = 0; i < nBuckets - 1; ++i) 
	{
		Bounds3f b0, b1;
		int count0 = 0, count1 = 0;
		for (int j = 0; j <= i; ++j) 
		{
			b0 = Union(b0, buckets[j].bounds);
			count0 += buckets[j].count;
		}
		for (int j = i + 1; j < nBuckets; ++j) 
		{
			b1 = Union(b1, buckets[j].bounds);
			count1 += buckets[j].count;
		}
		cost[i] = .125f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) / bounds.SurfaceArea();
	}

	// Find bucket to split at that minimizes SAH metric
	Float minCost = cost[0];
	int minCostSplitBucket = 0;
	for (int i = 1; i < nBuckets - 1; ++i) {
		if (cost[i] < minCost) {
			minCost = cost[i];
			minCostSplitBucket = i;
		}
	}

	// Split nodes and create interior HLBVH SAH node
	BVHBuildNode **pmid = std::partition(
		&treeletRoots[start], &treeletRoots[end - 1] + 1,
		[=](const BVHBuildNode *node) {
			Float centroid = (node->bounds.pMin[dim] + node->bounds.pMax[dim]) * 0.5f;
			int b = nBuckets * ((centroid - centroidBounds.pMin[dim]) / (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]));
			if (b == nBuckets) b = nBuckets - 1;
			//CHECK_GE(b, 0);
			//CHECK_LT(b, nBuckets);
			return b <= minCostSplitBucket;
		});
	int mid = pmid - &treeletRoots[0];
	//CHECK_GT(mid, start);
	//CHECK_LT(mid, end);
	node->InitInterior(dim, 
		buildUpperSAH(arena, treeletRoots, start, mid, totalNodes),
		buildUpperSAH(arena, treeletRoots, mid, end, totalNodes));
	return node;
}

BVHBuildNode* BVHAccel::recursiveBuild(MemoryArena& arena,
	std::vector<BVHPrimitiveInfo>& primitiveInfo,
	int start,
	int end,
	int *totalNodes,
	std::vector<std::shared_ptr<Primitive>> &orderedPrims)
{
	BVHBuildNode* node = arena.Alloc<BVHBuildNode>();
	(*totalNodes)++;
	Bounds3f bounds;
	for (int i = start; i < end; ++i)
	{
		bounds = Union(bounds, primitiveInfo[i].bounds);
	}
	int nPrimitives = end - start;
	if (nPrimitives == 1)
	{
		int firstPrimOffset = orderedPrims.size();
		for (int i = start; i < end; ++i)
		{
			int primNum = primitiveInfo[i].primitiveNumber;
			orderedPrims.push_back(primitives[primNum]);
		}
		node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
		return node;
	}
	else
	{
		Bounds3f centroidBounds;
		for (int i = start; i < end; ++i)
		{
			centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
		}
		int dim = centroidBounds.MaximumExtent();//�������Χ��ά��
		int mid = (start + end) / 2;
		if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim])//��ζ������bound�����ĵ��غ�
		{
			int firstPrimOffset = orderedPrims.size();
			for (int i = start; i < end; ++i)
			{
				int primNum = primitiveInfo[i].primitiveNumber;
				orderedPrims.push_back(primitives[primNum]);
			}
			node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
			return node;
		}
		else
		{
			switch (splitMethod)
			{
			case SplitMethod::Middle://�����Χ����ά�ȵ��е�Ϊ�ָ��
			{
				Float pmid = (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]) / 2;
				//std::partition��range[start,end)��������,ʹ��predicateΪtrue��Ԫ������predicateΪfalse��ǰ��,��������ԭ�е��Ⱥ��ϵ
				BVHPrimitiveInfo* midptr = std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
					[dim, pmid](const BVHPrimitiveInfo& pi)
					{
						return pi.centroid[dim] < pmid;
					});
				mid = midptr - &primitiveInfo[0];
				if (mid != start && mid != end) break;//(mid==start || mid == end)��ζ�����е�Ԫ�ض������ĵ�(��)һ��,��ʱ����EqualCounts����
			}
			case SplitMethod::EqualCounts://��������ƽ��
			{
				mid = (start + end) / 2;
				//std::nth_element��ı��n��Ԫ�ص�ֵʹ����ǰ�����С����(��predΪtrue)�������С����(��predΪfalse)
				std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
					[dim](const BVHPrimitiveInfo& a, const BVHPrimitiveInfo& b)
					{
						return a.centroid[dim] < b.centroid[dim];
					});
				break;
			}
			//������������㷨
			//����̰���㷨,������Ԫ�ذ��ռ�λ�û��ֳ�n��Ͱ,��Ͱ��������ϲ�����ÿ�ֻ��ֵĴ���,ѡ�������С�Ľ��еݹ�
			case SplitMethod::SAH:
			default:
			{
				if (nPrimitives <= 4)
				{
					mid = (start + end) / 2;
					//std::nth_element��ı��n��Ԫ�ص�ֵʹ����ǰ�����С����(��predΪtrue)�������С����(��predΪfalse)
					std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
						[dim](const BVHPrimitiveInfo& a, const BVHPrimitiveInfo& b)
						{
							return a.centroid[dim] < b.centroid[dim];
						});
				}
				else
				{
					//���ֳ�nBuckets��Ͱ
					constexpr int nBuckets = 12;
					struct BucketInfo
					{
						int count = 0;
						Bounds3f bounds;
					};
					BucketInfo buckets[nBuckets];
					//��primitive�������ĵ����ܷ�ΧcentroidBounds�ڵ�ƫ����װ��Ͱ��
					for (int i = start; i < end; ++i)
					{
						int b = nBuckets * centroidBounds.Offset(primitiveInfo[i].centroid)[dim];
						if (b == nBuckets) b = nBuckets - 1;
						buckets[b].count++;
						buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
					}
					//����ÿ��Ͱ����Ӧ��cost
					Float cost[nBuckets - 1];
					for (int i = 0; i < nBuckets - 1; ++i)
					{
						Bounds3f b0, b1;
						int count0 = 0, count1 = 0;
						for (int j = 0; j <= i; ++j)
						{
							b0 = Union(b0, buckets[j].bounds);
							count0 += buckets[j].count;
						}
						for (int j = i + 1; j < nBuckets; ++j)
						{
							b1 = Union(b1, buckets[j].bounds);
							count1 += buckets[j].count;
						}
						//���������Դ���Ϊ1/8,�ཻ�Ĵ���Ϊ1
						cost[i] = .125f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) / bounds.SurfaceArea();
					}
					//�ҵ���С���Ǹ�����
					Float minCost = cost[0];
					int minCostSplitBucket = 0;
					for (int i = 1; i < nBuckets - 1; ++i)
					{
						if (cost[i] < minCost)
						{
							minCost = cost[i];
							minCostSplitBucket = i;
						}
					}
					//���Ԫ��̫����߷ָ�Ĵ���С�ڱ�������Ҷ�ӵĴ���������ָ������Ҷ�ӽڵ�
					Float leafCost = nPrimitives;
					if (nPrimitives > maxPrimsInNode || minCost < leafCost)
					{
						BVHPrimitiveInfo* pmid = std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
							[=](const BVHPrimitiveInfo& pi)
							{
								int b = nBuckets * centroidBounds.Offset(pi.centroid)[dim];
								if (b == nBuckets) b = nBuckets - 1;
								return b <= minCostSplitBucket;
							});
						mid = pmid - &primitiveInfo[0];
					}
					else
					{
						int firstPrimOffset = orderedPrims.size();
						for (int i = start; i < end; ++i)
						{
							int primNum = primitiveInfo[i].primitiveNumber;
							orderedPrims.push_back(primitives[primNum]);
						}
						node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
						return node;
					}
				}
				break;
			}
			}//switch end
			node->InitInterior(dim,
				recursiveBuild(arena, primitiveInfo, start, mid, totalNodes, orderedPrims),
				recursiveBuild(arena, primitiveInfo, mid, end, totalNodes, orderedPrims));
		}
		return node;
	}
}

int BVHAccel::flattenBVHTree(BVHBuildNode* node, int* offset)
{
	LinearBVHNode* linearNode = &nodes[*offset];
	linearNode->bounds = node->bounds;
	int myOffset = (*offset)++;
	if (node->nPrimitives > 0)
	{
		linearNode->primitivesOffset = node->firstPrimOffset;
		linearNode->nPrimitives = node->nPrimitives;
	}
	else
	{
		linearNode->axis = node->splitAxis;
		linearNode->nPrimitives = 0;
		flattenBVHTree(node->children[0], offset);
		linearNode->secondChildOffset = flattenBVHTree(node->children[1], offset);
	}
	return myOffset;
}


std::shared_ptr<BVHAccel> CreateBVHAccelerator(std::vector<std::shared_ptr<Primitive>> prims ,const ParamSet &ps)
{
	int maxPrimsInNode =  ps.FindOneInt("maxnodeprims", 4);
	BVHAccel::SplitMethod splitMethod = BVHAccel::SplitMethod::SAH;
	return std::make_shared<BVHAccel>(std::move(prims), 4, splitMethod);
}
