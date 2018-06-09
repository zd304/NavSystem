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

	unsigned int GetGraphCount();

	NavGraph* GetGraphByID(unsigned int id);
private:
	std::vector<NavGraph*> mGraphs;
	unsigned int mVersion;
};

extern NavSystem* gNavSystem;

#endif
