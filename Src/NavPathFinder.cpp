#include "NavPathFinder.h"
#include "NavTriangle.h"
#include "NavMesh.h"

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
		cost.cost = neighbor->mDistance[i];
		cost.state = (void*)neighbor;

		adjacent->push_back(cost);
	}
}

void NavPathFinder::PrintStateInfo(void* state)
{

}

bool NavPathFinder::Solve(NavTriangle* start, NavTriangle* end, std::vector<NavTriangle*>* path, float* cost)
{
	int rst = mPather->Solve((void*)start, (void*)end, (std::vector<void*>*)path, cost);
	if (rst == micropather::MicroPather::SOLVED
		|| rst == micropather::MicroPather::START_END_SAME)
		return true;
	return false;
}