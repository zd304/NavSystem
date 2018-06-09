#ifndef __NAV_MESH_H__
#define __NAV_MESH_H__

#include "NavInc.h"

class NavTriangle;
struct NavEdge;
struct AdjacentEdgePair;

class NavMesh
{
public:
	NavMesh();
	NavMesh(Vector3* vertices, int vertexNum, unsigned int* indices, int indicesNum);
	~NavMesh();

	bool GetHeight(const Vector3& pos, float* height);

	unsigned int GetSize();

	unsigned int WriteTo(char* dest, unsigned int ptr);

	unsigned int ReadFrom(char* src, unsigned int ptr);

	void UpdateAdjacent(bool calcBounds = true);
private:
	bool IsTriangleAdjacent(NavTriangle* tri1, NavTriangle* tri2, AdjacentEdgePair* aep);
public:
	std::vector<NavTriangle*> mTriangles;
	std::vector<NavEdge*> mBounds;
};

void DecodeEdgeIndex(int edgeIndex, int* pointIndex1, int* pointIndex2);

#endif