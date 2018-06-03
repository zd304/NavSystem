#ifndef __NAV_MESH_H__
#define __NAV_MESH_H__

#include "NavInc.h"

class NavTriangle;
struct NavEdge;
struct AdjacentEdgePair;

class NavMesh
{
public:
	NavMesh(Vector3* vertices, int vertexNum, unsigned int* indices, int indicesNum);
	~NavMesh();

	void UpdateAdjacent();
private:
	bool IsTriangleAdjacent(NavTriangle* tri1, NavTriangle* tri2, AdjacentEdgePair* aep);
public:
	std::vector<NavTriangle*> mTriangles;
	std::vector<NavEdge*> mBounds;
};

#endif