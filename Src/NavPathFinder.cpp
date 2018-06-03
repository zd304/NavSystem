#include "NavPathFinder.h"
#include "NavTriangle.h"
#include "NavMesh.h"
#include "NavPhysics.h"

static const float MAX_COST = 1e4f;

NavPathFinder::NavPathFinder(NavMesh* mesh)
{
	mPather = new micropather::MicroPather(this);
	mMesh = mesh;
}

NavPathFinder::~NavPathFinder()
{
	SAFE_DELETE(mPather);
}

float NavPathFinder::LeastCostEstimate(void* stateStart, void* stateEnd)
{
	if (!stateStart || !stateEnd)
		return MAX_COST;

	NavTriangle* triStart = (NavTriangle*)stateStart;
	NavTriangle* triEnd = (NavTriangle*)stateEnd;

	float cost = (triStart->mCenter - triEnd->mCenter).Length();
	return cost;
}

void NavPathFinder::AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent)
{
	NavTriangle* tri = (NavTriangle*)state;
	for (size_t i = 0; i < tri->mNeighbors.size(); ++i)
	{
		NavTriangle* neighbor = tri->mNeighbors[i];
		
		micropather::StateCost cost;
		cost.cost = tri->mDistance[i];
		cost.state = (void*)neighbor;

		adjacent->push_back(cost);
	}
}

void NavPathFinder::PrintStateInfo(void* state)
{

}

bool NavPathFinder::Solve(const NavTriangle* start, const NavTriangle* end, std::vector<NavTriangle*>* path, float* cost)
{
	int rst = mPather->Solve((void*)start, (void*)end, (std::vector<void*>*)path, cost);
	if (rst == micropather::MicroPather::SOLVED
		|| rst == micropather::MicroPather::START_END_SAME)
		return true;
	return false;
}

bool NavPathFinder::Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost)
{
	NavTriangle* triStart = GetTriangleByPoint(start);
	NavTriangle* triEnd = GetTriangleByPoint(end);
	std::vector<NavTriangle*> triPath;
	bool rst = Solve(triStart, triEnd, &triPath, cost);
	if (rst)
	{
		path->push_back(start);
		size_t nodeCount = triPath.size();
		for (size_t i = 0; i < nodeCount; ++i)
		{
			NavTriangle* tri = triPath[i];
			path->push_back(tri->mCenter);

			if (i < (nodeCount - 1))
			{
				NavTriangle* triNext = triPath[i + 1];
				for (size_t j = 0; j < tri->mNeighbors.size(); ++j)
				{
					if (tri->mNeighbors[j] != tri)
						continue;
					path->push_back(tri->mEdgeCenter[j]);
					break;
				}
			}
		}
		path->push_back(end);
	}
	return rst;
}

NavTriangle* NavPathFinder::GetTriangleByPoint(const Vector3& point)
{
	for (size_t i = 0; i < mMesh->mTriangles.size(); ++i)
	{
		NavTriangle* tri = mMesh->mTriangles[i];
		Vector3 v0 = tri->mPoint[0];
		Vector3 v1 = tri->mPoint[1];
		Vector3 v2 = tri->mPoint[2];

		if (!NavPhysics::TriangleIntersect2DPoint(v0, v1, v2, point))
			continue;
		NavPhysics::NavHit hitInfo;
		Vector3 orig(point.x, 1000.0f, point.z);
		if (!NavPhysics::RayIntersectTriangle(orig, Vector3::DOWN, v0, v1, v2, &hitInfo))
			continue;
		return tri;
	}
	return NULL;
}