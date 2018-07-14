#ifndef __NAV_SYSTEM_H__
#define __NAV_SYSTEM_H__

#include "NavInc.h"

namespace Nav
{
	class NavGraph;

	class NavSystem
	{
	public:
		NavSystem();
		~NavSystem();

		bool LoadFromFileW(const wchar_t* path);

		bool LoadFromFile(const char* path);

		bool LoadFromMemory(char* data);

		void SaveAs(const char* path);

		void AddGraph(NavGraph* graph);

		void Clear();

		unsigned int GetGraphCount();

		NavGraph* GetGraphByID(unsigned int id);

		void GetBound(Vector3* min, Vector3* max);

		unsigned int GetVersion();
	private:
		std::vector<NavGraph*> mGraphs;
		unsigned int mVersion;
	};

	extern NavSystem* gNavSystem;
}

#endif
