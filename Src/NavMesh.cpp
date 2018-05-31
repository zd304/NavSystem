#include "NavMesh.h"
#include "NavTriangle.h"

NavMesh::NavMesh(Vector3* vertices, int vertexNum, unsigned int* indices, int indicesNum)
{
	for (int i = 0; i < indicesNum; i+=3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i+1];
		unsigned int index2 = indices[i+2];

		NavTriangle* tri = new NavTriangle(vertices[index0], vertices[index1], vertices[index2]);
		mTriangles.push_back(tri);
	}
	UpdateAdjacent();
}

void NavMesh::UpdateAdjacent()
{
	for (size_t i = 0; i < mTriangles.size(); ++i)
	{
		NavTriangle* tri = mTriangles[i];
		tri->mNeighbors.clear();
		tri->mDistance.clear();
	}
	for (size_t i = 0; i < mTriangles.size(); ++i)
	{
		NavTriangle* tri1 = mTriangles[i];
		for (size_t j = i + 1; j < mTriangles.size(); ++j)
		{
			NavTriangle* tri2 = mTriangles[j];
			if (IsTriangleAdjacent(tri1, tri2))
			{
				Vector3 edge = tri1->mCenter - tri2->mCenter;
				float distance = edge.Length();

				tri1->mNeighbors.push_back(tri2);
				tri1->mDistance.push_back(distance);

				tri2->mNeighbors.push_back(tri1);
				tri2->mDistance.push_back(distance);
			}
		}
	}
}

bool NavMesh::IsTriangleAdjacent(NavTriangle* tri1, NavTriangle* tri2)
{
	int firstIndex = -1;
	for (int i = 0; i < 3; ++i)
	{
		Vector3& p1 = tri1->mPoint[i];
		for (int j = 0; j < 3; ++j)
		{
			if (firstIndex == j)
				continue;
			Vector3& p2 = tri2->mPoint[j];
			if (p1 == p2)
			{
				if (firstIndex >= 0)
					return true;
				else
					firstIndex = j;
				break;
			}
		}
	}
	return false;
}

NavMesh::~NavMesh()
{
	for (size_t i = 0; i < mTriangles.size(); ++i)
	{
		NavTriangle* tri = mTriangles[i];
		SAFE_DELETE(tri);
	}
	mTriangles.clear();
}