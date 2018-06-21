#ifndef __NAV_GATE_H__
#define __NAV_GATE_H__

#include "NavInc.h"

namespace Nav
{
	class NavTriangle;
	class NavGraph;
	struct NavEdge;

	class NavGate
	{
	public:
		NavGate(NavGraph* graph);
		~NavGate();

		void AddTriangle(const NavTriangle* tri);

		void SwitchPassable(bool passable);

		void CalcBounds();

		unsigned int GetSize();

		unsigned int WriteTo(char* dest, unsigned int ptr);

		unsigned int ReadFrom(char* src, unsigned int ptr);
	private:
		void ClearBounds();
	public:
		unsigned int mID;
		std::vector<unsigned int> mTriIndices;
		NavGraph* mNavGraph;
		bool mPassable;
		std::vector<NavEdge*> mBounds;
	};
}

#endif