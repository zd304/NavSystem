#include "NavSystem.h"
#include "NavMesh.h"
#include "NavHeightmap.h"
#include "NavGraph.h"
//#include <stdio.h>
#include <fstream>

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace Nav
{
	NavSystem* gNavSystem = NULL;

	NavSystem::NavSystem()
	{
		mVersion = 101;
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
		if (graph->mID == 0)
		{
			graph->mID = (unsigned int)mGraphs.size();
		}
		mGraphs.push_back(graph);
		mGraphsMap[graph->mID] = graph;
	}

	void NavSystem::ResetGraphID(unsigned int oldId, unsigned int newId)
	{
		std::map<unsigned int, NavGraph*>::iterator it;
		it = mGraphsMap.find(oldId);
		if (it == mGraphsMap.end())
			return;
		NavGraph* graph = it->second;
		graph->mID = newId;
		mGraphsMap.erase(it);

		mGraphsMap[newId] = graph;
	}

	unsigned int NavSystem::GetGraphCount()
	{
		return (unsigned int)mGraphs.size();
	}

	NavGraph* NavSystem::GetGraphByID(unsigned int id)
	{
		std::map<unsigned int, NavGraph*>::iterator it;
		it = mGraphsMap.find(id);
		if (it == mGraphsMap.end())
			return NULL;
		return it->second;
	}

	NavGraph* NavSystem::GetGraphByIndex(unsigned int index)
	{
		if (index < 0 || mGraphs.size() <= index)
			return NULL;
		return mGraphs[index];
	}

	void NavSystem::GetBound(Vector3* min, Vector3* max)
	{
		if (min == NULL || max == NULL)
			return;
		min->Set(FLT_MAX, FLT_MAX, FLT_MAX);
		max->Set(FLT_MIN, FLT_MIN, FLT_MIN);
		for (size_t i = 0; i < mGraphs.size(); ++i)
		{
			NavGraph* graph = mGraphs[i];
			if (graph == NULL || graph->mMesh == NULL)
				continue;
			Vector3 nmMax, nmMin;
			graph->mMesh->GetBound(&nmMin, &nmMax);
			min->Set(
				MinFloat(nmMin.x, min->x),
				MinFloat(nmMin.y, min->y),
				MinFloat(nmMin.z, min->z)
			);
			max->Set(
				MaxFloat(nmMax.x, max->x),
				MaxFloat(nmMax.y, max->y),
				MaxFloat(nmMax.z, max->z)
			);
		}
	}

	bool NavSystem::LoadFromFileW(const wchar_t* path)
	{
		Clear();

#ifdef _WINDOWS
		std::ifstream stream;
		stream.open(path, std::ios::in | std::ios::binary);
		if (!stream.is_open())
			return false;
		stream.seekg(0, std::ios::end);
		long long fileSize = stream.tellg();
		stream.seekg(0, std::ios::beg);

		char* data = new char[fileSize];
		stream.read(data, fileSize);
		stream.close();

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
		SAFE_DELETE_ARRAY(data);
		return true;
#else
		return false;
#endif
	}

	bool NavSystem::LoadFromFile(const char* path)
	{
		Clear();

		FILE* fp = fopen(path, "rb");
		if (fp == NULL)
			return false;
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
		SAFE_DELETE_ARRAY(data);
		return true;
	}

	bool NavSystem::LoadFromMemory(char* data)
	{
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

		return true;
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
}