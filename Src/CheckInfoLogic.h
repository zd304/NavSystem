#ifndef __CHECK_INFO_LOGIC_H__
#define __CHECK_INFO_LOGIC_H__

#include "inc.h"

class NavTriangle;
class MeshRenderer;
class NavGraph;
class Test;

class CheckInfoLogic
{
public:
	CheckInfoLogic(Test* test);
	~CheckInfoLogic();

	void OnPick(const NavTriangle* tri, const Vector3& point, const NavGraph* graph);

	void OnGUI();
private:
	Test* mTest;
	NavGraph* mGraph;
	NavTriangle* mTri;
};

#endif
