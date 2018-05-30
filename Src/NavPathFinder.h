#ifndef __NAV_PATH_FINDER_H__
#define __NAV_PATH_FINDER_H__

#include "NavInc.h"

class NavMesh;
class NavTriangle;

class NavPathFinder : public micropather::Graph
{
public:
	NavPathFinder(NavMesh* mesh);
	~NavPathFinder();

	bool Solve(NavTriangle* start, NavTriangle* end, std::vector<NavTriangle*>* path, float* cost);
public:
	// Inherit;
	float LeastCostEstimate(void* stateStart, void* stateEnd);
	void AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent);
	void PrintStateInfo(void* state);
public:
	NavMesh* mMesh;
	micropather::MicroPather* mPather;
};

#endif