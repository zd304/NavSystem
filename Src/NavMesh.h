#ifndef __NAV_MESH_H__
#define __NAV_MESH_H__

#include "NavInc.h"

class NavTriangle;

class NavMesh
{
public:
	NavMesh(Vector3* vertices, int vertexNum, unsigned int* indices, int indicesNum);
	~NavMesh();

	void UpdateAdjacent();
private:
	bool IsTriangleAdjacent(NavTriangle* tri1, NavTriangle* tri2);
public:
	std::vector<NavTriangle*> mTriangles;
};

#endif