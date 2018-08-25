#ifndef __NAV_SYSTEM_H__
#define __NAV_SYSTEM_H__

#include "NavInc.h"

namespace Nav
{
	class NavGraph;
	class NavSceneTree;

	class NavSystem
	{
	public:
		NavSystem();
		~NavSystem();

		void InitSceneTree(float x, float y, float width, float height, int maxLevel);

		bool LoadFromFileW(const wchar_t* path);

		bool LoadFromFile(const char* path);

		bool LoadFromMemory(char* data);

		void SaveAs(const char* path);

		void AddGraph(NavGraph* graph);

		void ResetGraphID(unsigned int oldId, unsigned int newId);

		bool Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost, bool smoothPath) const;

		void Clear();

		unsigned int GetGraphCount();

		NavGraph* GetGraphByID(unsigned int id);

		NavGraph* GetGraphByIndex(unsigned int index);

		void GetBound(Vector3* min, Vector3* max);

		unsigned int GetVersion();
	private:
		std::vector<NavGraph*> mGraphs;
		std::map<unsigned int, NavGraph*> mGraphsMap;
		NavSceneTree* mScene;
		unsigned int mVersion;
	};

	extern NavSystem* gNavSystem;
}

#endif
