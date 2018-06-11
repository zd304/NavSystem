#include "NavGate.h"
#include "NavTriangle.h"
#include "NavMesh.h"
#include "NavHeightmap.h"
#include "NavGraph.h"
#include "NavEdge.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

NavGate::NavGate(NavGraph* graph)
{
	mNavGraph = graph;
	mPassable = true;
}

NavGate::~NavGate()
{
	ClearBounds();
}

void NavGate::AddTriangle(const NavTriangle* tri)
{
	NavMesh* mesh = mNavGraph->mMesh;
	if (!mesh) return;
	
	for (unsigned int i = 0; i < mesh->mTriangles.size(); ++i)
	{
		NavTriangle* t = mesh->mTriangles[i];
		if (t == tri)
		{
			mTriIndices.push_back(i);
			return;
		}
	}
}

void NavGate::SwitchPassable(bool passable)
{
	NavMesh* mesh = mNavGraph->mMesh;
	if (!mesh) return;
	NavHeightmap* heightmap = mNavGraph->mHeightmap;
	if (!heightmap) return;
	for (unsigned int i = 0; i < mTriIndices.size(); ++i)
	{
		unsigned int triIndex = mTriIndices[i];
		NavTriangle* tri = mesh->mTriangles[triIndex];
		heightmap->SwitchCellPassability(tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], passable);
		tri->mPassable = passable;
	}
	mNavGraph->ResetCost();
	if (!passable)
	{
		CalcBounds();
	}
	else
	{
		ClearBounds();
	}
}

void NavGate::CalcBounds()
{
	ClearBounds();
	if (!mNavGraph->mMesh)
		return;

	for (unsigned int i = 0; i < mTriIndices.size(); ++i)
	{
		unsigned int triIndex = mTriIndices[i];
		NavTriangle* tri = mNavGraph->mMesh->mTriangles[triIndex];
		for (unsigned int j = 0; j < tri->mNeighbors.size(); ++j)
		{
			NavTriangle* neighbor = tri->mNeighbors[j];
			if (!neighbor->mPassable)
				continue;
			int edgeIndex = tri->mShareEdgeIndices[j];
			
			int ptIndex1 = -1;
			int ptIndex2 = -1;
			DecodeEdgeIndex(edgeIndex, &ptIndex1, &ptIndex2);
			NavEdge* edge = new NavEdge();
			edge->mPoint[0] = tri->mPoint[ptIndex1];
			edge->mPoint[1] = tri->mPoint[ptIndex2];
			mBounds.push_back(edge);
		}
	}
}

void NavGate::ClearBounds()
{
	for (unsigned int i = 0; i < mBounds.size(); ++i)
	{
		NavEdge* edge = mBounds[i];
		SAFE_DELETE(edge);
	}
	mBounds.clear();
}

unsigned int NavGate::GetSize()
{
	unsigned int size = sizeof(unsigned int)* (unsigned int)mTriIndices.size();
	size += sizeof(unsigned int);
	size += sizeof(bool);
	return size;
}

unsigned int NavGate::WriteTo(char* dest, unsigned int ptr)
{
	unsigned int count = (unsigned int)mTriIndices.size();
	memcpy(dest + ptr, &count, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	unsigned int size = sizeof(unsigned int)* (unsigned int)mTriIndices.size();
	memcpy(dest + ptr, &mTriIndices[0], size);
	ptr += size;
	memcpy(dest + ptr, &mPassable, sizeof(bool));
	ptr += sizeof(bool);

	return ptr;
}

unsigned int NavGate::ReadFrom(char* src, unsigned int ptr)
{
	mTriIndices.clear();

	unsigned int count = 0;
	memcpy(&count, src + ptr, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	for (unsigned int i = 0; i < count; ++i)
	{
		unsigned int index = 0;
		memcpy(&index, src + ptr, sizeof(unsigned int));
		mTriIndices.push_back(index);
		ptr += sizeof(unsigned int);
	}
	memcpy(&mPassable, src + ptr, sizeof(bool));
	ptr += sizeof(bool);

	SwitchPassable(mPassable);

	return ptr;
}