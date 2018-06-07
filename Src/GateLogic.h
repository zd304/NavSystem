#ifndef __GATE_LOGIC_H__
#define __GATE_LOGIC_H__

#include "inc.h"

class Test;
class NavGraph;
class NavGate;
class NavTriangle;

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

	void OnPick(const NavTriangle* tri, const Vector3& point, const NavGraph* graph);
private:
	Test* mTest;
	NavGraph* mNavGraph;
	NavGate* mNavGate;

	GateEditMode mCurMode;
};

#endif