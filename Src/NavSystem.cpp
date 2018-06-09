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
	for (unsigned int i = 0; i < mGraphs.size(); ++i)
	{
		NavGraph* graph = mGraphs[i];
		SAFE_DELETE(graph);
	}
	mGraphs.clear();
}

void NavSystem::AddGraph(NavGraph* graph)
{
	graph->mID = (unsigned int)mGraphs.size();
	mGraphs.push_back(graph);
}

unsigned int NavSystem::GetGraphCount()
{
	return (unsigned int)mGraphs.size();
}

NavGraph* NavSystem::GetGraphByID(unsigned int id)
{
	return mGraphs[id];
}

void NavSystem::LoadFromFile(const char* path)
{
	Clear();

	FILE* fp = fopen(path, "rb");
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
		NavGraph* graph = new NavGraph();
		ptr = graph->ReadFrom(data, ptr);
		AddGraph(graph);
	}
}

void NavSystem::SaveAs(const char* path)
{
	unsigned int fileSize = sizeof(unsigned int);
	fileSize += sizeof(unsigned int);
	for (unsigned int i = 0; i < mGraphs.size(); ++i)
	{
		NavGraph* graph = mGraphs[i];
		fileSize += graph->GetSize();
	}

	char* data = new char[fileSize];
	unsigned int ptr = 0;
	memcpy(data + ptr, &mVersion, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	unsigned int graphCount = (unsigned int)mGraphs.size();
	memcpy(data + ptr, &graphCount, sizeof(unsigned int));
	ptr += sizeof(unsigned int);

	for (unsigned int i = 0; i < mGraphs.size(); ++i)
	{
		NavGraph* graph = mGraphs[i];
		ptr = graph->WriteTo(data, ptr);
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

unsigned int NavSystem::GetVersion()
{
	return mVersion;
}