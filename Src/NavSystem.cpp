#include "NavSystem.h"
#include "NavMesh.h"
#include "NavHeightmap.h"
#include "NavGraph.h"
#include <stdio.h>

NavSystem* gNavSystem = NULL;

NavSystem::NavSystem()
{
	mVersion = 100;
}

NavSystem::~NavSystem()
{
	Clear();
}

void NavSystem::Clear()
{
	for (size_t i = 0; i < mGraphs.size(); ++i)
	{
		NavGraph* graph = mGraphs[i];
		SAFE_DELETE(graph);
	}
	mGraphs.clear();
}

void NavSystem::AddGraph(NavGraph* graph)
{
	graph->mID = mGraphs.size();
	mGraphs.push_back(graph);
}

size_t NavSystem::GetGraphCount()
{
	return mGraphs.size();
}

NavGraph* NavSystem::GetGraphByID(size_t id)
{
	return mGraphs[id];
}

void NavSystem::LoadFromFile(const char* path)
{
	Clear();

	FILE* fp = fopen(path, "r");
	if (fp == NULL)
		return;
	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* data = new char[fileSize];
	fread(data, 1, fileSize, fp);
	fclose(fp);

	mGraphs.clear();

	unsigned int ptr = 0;
	memcpy(&mVersion, data + ptr, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	unsigned int graphCount = 0;
	memcpy(&graphCount, data + ptr, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	for (unsigned int i = 0; i < graphCount; ++i)
	{
		NavMesh* navMesh = new NavMesh();
		ptr = navMesh->ReadFrom(data, ptr);

		NavHeightmap* navHeightmap = new NavHeightmap();
		ptr = navHeightmap->ReadFrom(data, ptr);

		NavGraph* graph = new NavGraph();
		graph->mMesh = navMesh;
		graph->mHeightmap = navHeightmap;
		AddGraph(graph);
	}
}

void NavSystem::SaveAs(const char* path)
{
	unsigned int fileSize = sizeof(unsigned int);
	fileSize += sizeof(unsigned int);
	for (size_t i = 0; i < mGraphs.size(); ++i)
	{
		NavGraph* graph = mGraphs[i];
		fileSize += graph->mMesh->GetSize();
		fileSize += graph->mHeightmap->GetSize();
	}

	char* data = new char[fileSize];
	unsigned int ptr = 0;
	memcpy(data + ptr, &mVersion, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	unsigned int graphCount = mGraphs.size();
	memcpy(data + ptr, &graphCount, sizeof(unsigned int));
	ptr += sizeof(unsigned int);

	for (size_t i = 0; i < mGraphs.size(); ++i)
	{
		NavGraph* graph = mGraphs[i];
		ptr = graph->mMesh->WriteTo(data, ptr);
		ptr = graph->mHeightmap->WriteTo(data, ptr);
	}

	FILE* fp = fopen(path, "wb");
	if (!fp)
	{
		SAFE_DELETE_ARRAY(data);
		return;
	}
	fwrite(data, 1, fileSize, fp);
	fclose(fp);

	SAFE_DELETE_ARRAY(data);
}