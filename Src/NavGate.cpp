#include "NavGate.h"
#include "NavTriangle.h"
#include "NavMesh.h"
#include "NavHeightmap.h"
#include "NavGraph.h"

NavGate::NavGate(NavGraph* graph)
{
	mNavGraph = graph;
	mPassable = true;
}

void NavGate::AddTriangle(const NavTriangle* tri)
{
	NavMesh* mesh = mNavGraph->mMesh;
	if (!mesh) return;
	
	for (size_t i = 0; i < mesh->mTriangles.size(); ++i)
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
	for (size_t i = 0; i < mTriIndices.size(); ++i)
	{
		unsigned int triIndex = mTriIndices[i];
		NavTriangle* tri = mesh->mTriangles[triIndex];
		heightmap->SwitchCellPassability(tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], passable);
		tri->mPassable = passable;
	}
	mNavGraph->ResetCost();
}

unsigned int NavGate::GetSize()
{
	unsigned int size = sizeof(unsigned int)* mTriIndices.size();
	size += sizeof(unsigned int);
	size += sizeof(bool);
	return size;
}

unsigned int NavGate::WriteTo(char* dest, unsigned int ptr)
{
	size_t count = mTriIndices.size();
	memcpy(dest + ptr, &count, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	size_t size = sizeof(unsigned int)* mTriIndices.size();
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
	for (size_t i = 0; i < count; ++i)
	{
		unsigned int index = 0;
		memcpy(&index, src + ptr, sizeof(unsigned int));
		mTriIndices.push_back(index);
		ptr += sizeof(unsigned int);
	}
	memcpy(&mPassable, src + ptr, sizeof(bool));
	ptr += sizeof(bool);

	return ptr;
}