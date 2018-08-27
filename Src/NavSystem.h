#ifndef __NAV_SYSTEM_H__
#define __NAV_SYSTEM_H__

#include "NavInc.h"

namespace Nav
{
	class NavGraph;
	class NavSceneTree;
	class NavSceneNode;

	class NavSystem
	{
	public:
		NavSystem();
		~NavSystem();

		void InitSceneTree(float x, float y, float width, float height, int maxLevel);

		bool AddSceneNode(NavSceneNode* node);

		bool LoadFromFileW(const wchar_t* path);

		bool LoadFromFile(const char* path);

		bool LoadFromMemory(char* data);

		bool LoadSketchScneneFromFile(const char* path);

		bool LoadSketchScneneFromMemory(char* data);

		void SaveAs(const char* path);

		void AddGraph(NavGraph* graph);

		void ResetGraphID(unsigned int oldId, unsigned int newId);

		void Clear();

		unsigned int GetGraphCount();

		NavGraph* GetGraphByID(unsigned int id);

		NavGraph* GetGraphByIndex(unsigned int index);

		void GetBound(Vector3* min, Vector3* max);

		unsigned int GetVersion();
	private:
		std::vector<NavGraph*> mGraphs;
		std::map<unsigned int, NavGraph*> mGraphsMap;
		unsigned int mVersion;
	public:
		NavSceneTree* mScene;
	};

	extern NavSystem* gNavSystem;
}

#endif
