#ifndef __TEST_H__
#define __TEST_H__

#include "inc.h"

class MeshRenderer;
class NavMesh;
class NavTriangle;
class NavPathFinder;

enum eClickMode
{
	None,
	Start,
	End
};

class Test
{
public:
	Test();
	~Test();

	void OnInit(HWND hwnd, IDirect3DDevice9* device);

	void OnUpdate();

	void OnGUI();

	void OnQuit();

	void Pick(int x, int y);

	void AddSelectedTriangle(NavTriangle* tri);
private:
	void GetWorldRay(IDirect3DDevice9* pDevice, long x, long y, long width, long height, Vector3& orig, Vector3& dir);

	bool IsTriangleInSameMesh(NavTriangle* tri1, NavTriangle* tri2, NavPathFinder*& outFinder);

	void SetPointMesh(NavTriangle* tri, bool isStart);

	void ClearPath();
public:
	IDirect3DDevice9* mDevice;
	HWND mHwnd;
	int mWidth;
	int mHeight;
	MeshRenderer* mRenderer;
	D3DMATERIAL9 mMatNav;

	std::vector<NavPathFinder*> mNavMeshes;
	std::vector<NavTriangle*> mSelectedTriangles;
	ID3DXMesh* mSelectedMesh;

	ID3DXMesh* mStartMesh;
	ID3DXMesh* mEndMesh;
	NavTriangle* mStartTri;
	NavTriangle* mEndTri;

	eClickMode mClickMode;
};

#endif