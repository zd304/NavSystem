#ifndef __TEST_H__
#define __TEST_H__

#include "inc.h"

class MeshRenderer;
class NavMesh;
class NavTriangle;
class NavGraph;
class NavSystem;
class FileDialog;
class PathFindLogic;
class GateLogic;

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
public:
	static Test* GetInstance();
private:
	void GetWorldRay(IDirect3DDevice9* pDevice, long x, long y, long width, long height, Vector3& orig, Vector3& dir);

	bool IsTriangleInSameMesh(NavTriangle* tri1, NavTriangle* tri2, NavGraph*& outFinder);

	void UpdateView();

	void TransformPos(Vector3& pos);

	void OnMenu();

	void OnInput();

	void OpenFBX(const char* filePath);

	void OpenNav(const char* filePath);

	void CloseFile();
public:
	MeshRenderer* mRenderer;

private:
	IDirect3DDevice9* mDevice;
	HWND mHwnd;
	int mWidth;
	int mHeight;
	NavSystem* mNavSystem;

	PathFindLogic* mPathFindLogic;
	GateLogic* mGateLogic;

	// FileDialog
	FileDialog* mOpenFBX;
	FileDialog* mOpenNav;
	FileDialog* mSaveNav;

	// Camera
	float mCameraDistance;
	float mCameraHeight;
	float mCameraX;// -boxSize;
	D3DXMATRIX mWorldMtrix;
	float rot;

	static Test* mInstance;
};

#endif