#ifndef __TEST_H__
#define __TEST_H__

#include "inc.h"

class MeshRenderer;
class NavMesh;
class NavTriangle;
class NavPathFinder;

enum eClickState
{
	eClickState_None,
	eClickState_Start,
	eClickState_End
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
private:
	void GetWorldRay(IDirect3DDevice9* pDevice, long x, long y, long width, long height, Vector3& orig, Vector3& dir);

	bool IsTriangleInSameMesh(NavTriangle* tri1, NavTriangle* tri2, NavPathFinder*& outFinder);

	void SetPointMesh(NavTriangle* tri, const Vector3& point, bool isStart);

	void AddSelectedTriangle(NavTriangle* tri);

	void ClearPath();

	void UpdateView();

	void TransformPos(Vector3& pos);
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
	IDirect3DVertexBuffer9* mSelectedVB;
	IDirect3DIndexBuffer9* mSelectedIB;
	std::vector<Vector3> mSelectedPath;

	ID3DXMesh* mStartMesh;
	ID3DXMesh* mEndMesh;
	NavTriangle* mStartTri;
	NavTriangle* mEndTri;
	Vector3 mStartPoint;
	Vector3 mEndPoint;

	eClickState mClickMode;

	// Camera
	float mCameraDistance;
	float mCameraHeight;
	float mCameraX;// -boxSize;

	D3DXMATRIX mWorldMtrix;
	float rot;
};

#endif