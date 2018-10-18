#ifndef __EDGE_LOGIC_H__
#define __EDGE_LOGIC_H__

#include "inc.h"

namespace Nav
{
	class NavTriangle;
	class NavGraph;
	struct NavEdge;
}
class MeshRenderer;
class Test;

class EdgeLogic
{
public:
	EdgeLogic(Test* test);
	~EdgeLogic();

	void OnPick(const Nav::NavTriangle* tri, const Nav::Vector3& point, const Nav::NavGraph* graph);

	void OnGUI();

	void Clear(); 
public:
	Test* mTest;
	Nav::NavGraph* mGraph;
	std::vector<Nav::NavEdge*> mDelBounds;
	size_t selBoundsIndex;
};

#endif
