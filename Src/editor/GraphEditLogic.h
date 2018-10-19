#ifndef __GRAPH_EDIT_LOGIC_H__
#define __GRAPH_EDIT_LOGIC_H__

#include "inc.h"

namespace Nav
{
	class NavTriangle;
	class NavGraph;
}
class MeshRenderer;
class Test;

class GraphEditLogic
{
public:
	GraphEditLogic(Test* test);
	~GraphEditLogic();

	void OnPick(const Nav::NavTriangle* tri, const Nav::Vector3& point, const Nav::NavGraph* graph);

	void OnGUI();
private:
	Test* mTest;

	Nav::NavGraph* mGraph;
	unsigned int mID;
};

#endif
