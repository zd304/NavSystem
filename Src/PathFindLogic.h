#ifndef __PATH_FIND_MODE__
#define __PATH_FIND_MODE__

#include "inc.h"

namespace Nav
{
	class NavTriangle;
	class NavGraph;
}
class MeshRenderer;
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

	void OnPick(const Nav::NavTriangle* tri, const Vector3& point, const Nav::NavGraph* graph);

	void OnGUI();
private:
	void ClearPath();

	void SetPointMesh(const Nav::NavTriangle* tri, const Vector3& point, bool isStart);
public:
	bool mShowTriPath;
private:
	Test* mTest;

	Nav::NavTriangle* mStartTri;
	Nav::NavTriangle* mEndTri;
	Vector3 mStartPoint;
	Vector3 mEndPoint;
	float mCost;

	eClickState mClickMode;
	bool mSmoothPath;
};

#endif
