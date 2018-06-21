#ifndef __CHECK_INFO_LOGIC_H__
#define __CHECK_INFO_LOGIC_H__

#include "inc.h"

namespace Nav
{
	class NavTriangle;
	class NavGraph;
}
class MeshRenderer;
class Test;

class CheckInfoLogic
{
public:
	CheckInfoLogic(Test* test);
	~CheckInfoLogic();

	void OnPick(const Nav::NavTriangle* tri, const Vector3& point, const Nav::NavGraph* graph);

	void OnGUI();
private:
	Test* mTest;
	Nav::NavGraph* mGraph;
	Nav::NavTriangle* mTri;
};

#endif
