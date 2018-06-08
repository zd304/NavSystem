#ifndef __NAV_GATE_H__
#define __NAV_GATE_H__

#include "NavInc.h"

class NavTriangle;
class NavGraph;

class NavGate
{
public:
	NavGate(NavGraph* graph);

	void AddTriangle(const NavTriangle* tri);

	void SwitchPassable(bool passable);

	unsigned int GetSize();

	unsigned int WriteTo(char* dest, unsigned int ptr);

	unsigned int ReadFrom(char* src, unsigned int ptr);
public:
	unsigned int mID;
	std::vector<unsigned int> mTriIndices;
	NavGraph* mNavGraph;
	bool mPassable;
};

#endif