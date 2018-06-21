#ifndef __GATE_LOGIC_H__
#define __GATE_LOGIC_H__

#include "inc.h"

namespace Nav
{
	class NavGraph;
	class NavGate;
	class NavTriangle;
}
class Test;

enum GateEditMode
{
	GateEditMode_GateList,
	GateEditMode_GateTriangles
};

class GateLogic
{
public:
	GateLogic(Test* test);
	~GateLogic();

	void OnGUI();

	void OnPick(const Nav::NavTriangle* tri, const Vector3& point, const Nav::NavGraph* graph);
private:
	Test* mTest;
	Nav::NavGraph* mNavGraph;
	Nav::NavGate* mNavGate;

	GateEditMode mCurMode;
};

#endif