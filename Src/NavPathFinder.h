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

	bool Solve(const NavTriangle* start, const NavTriangle* end, std::vector<NavTriangle*>* path, float* cost);

	bool Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost);

	bool LineTest(const Vector3& start, const Vector3& end, Vector3& hitPoint);

	bool IsLineTest(const Vector3& start, const Vector3& end);
public:
	// Inherit;
	float LeastCostEstimate(void* stateStart, void* stateEnd);
	void AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent);
	void PrintStateInfo(void* state);
private:
	NavTriangle* GetTriangleByPoint(const Vector3& point);

	void SmoothPath(std::vector<Vector3>* path);
public:
	NavMesh* mMesh;
	micropather::MicroPather* mPather;
};

#endif