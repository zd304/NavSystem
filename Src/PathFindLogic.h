#ifndef __PATH_FIND_MODE__
#define __PATH_FIND_MODE__

#include "inc.h"

class NavTriangle;
class MeshRenderer;
class NavGraph;
class Test;

enum eClickState
{
	eClickState_None,
	eClickState_Start,
	eClickState_End
};

class PathFindLogic
{
public:
	PathFindLogic(Test* test);
	~PathFindLogic();

	void OnPick(const NavTriangle* tri, const Vector3& point, const NavGraph* graph);
private:
	void ClearPath();

	void SetPointMesh(const NavTriangle* tri, const Vector3& point, bool isStart);
private:
	Test* mTest;

	NavTriangle* mStartTri;
	NavTriangle* mEndTri;
	Vector3 mStartPoint;
	Vector3 mEndPoint;

	eClickState mClickMode;
};

#endif
