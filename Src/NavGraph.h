#ifndef __NAV_PATH_FINDER_H__
#define __NAV_PATH_FINDER_H__

#include "NavInc.h"

class NavMesh;
class NavTriangle;
class NavHeightmap;
class NavGate;

class NavGraph : public micropather::Graph
{
public:
	NavGraph();
	NavGraph(NavMesh* mesh);
	~NavGraph();

	bool Solve(const NavTriangle* start, const NavTriangle* end, std::vector<NavTriangle*>* path, float* cost) const;

	bool Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost, bool smoothPath = true) const;

	bool LineTest(const Vector3& start, const Vector3& end, Vector3& hitPoint) const;

	bool IsLineTest(const Vector3& start, const Vector3& end) const;

	void ResetCost();

	unsigned int GetSize();

	unsigned int WriteTo(char* dest, unsigned int ptr);

	unsigned int ReadFrom(char* src, unsigned int ptr);
public:
	// Inherit;
	float LeastCostEstimate(void* stateStart, void* stateEnd);
	void AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent);
	void PrintStateInfo(void* state);
private:
	NavTriangle* GetTriangleByPoint(const Vector3& point) const;

	void SmoothPath(std::vector<Vector3>* path) const;
public:
	NavMesh* mMesh;
	NavHeightmap* mHeightmap;
	std::vector<NavGate*> mGates;
	micropather::MicroPather* mPather;
	unsigned int mID;
};

#endif