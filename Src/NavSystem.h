#ifndef __NAV_SYSTEM_H__
#define __NAV_SYSTEM_H__

#include "NavInc.h"

class NavGraph;

class NavSystem
{
public:
	NavSystem();
	~NavSystem();

	void LoadFromFile(const char* path);

	void SaveAs(const char* path);

	void AddGraph(NavGraph* graph);

	void Clear();

	size_t GetGraphCount();

	NavGraph* GetGraphByID(size_t id);
private:
	std::vector<NavGraph*> mGraphs;
	unsigned int mVersion;
};

extern NavSystem* gNavSystem;

#endif
