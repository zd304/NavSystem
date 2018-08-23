#ifndef __TEST_H__
#define __TEST_H__

#include "inc.h"

class MeshRenderer;
namespace Nav
{
	class NavMesh;
	class NavTriangle;
	class NavGraph;
	class NavSystem;
}
class FileDialog;
class PathFindLogic;
class GateLogic;
class CheckInfoLogic;
class GraphEditLogic;
class Camera;

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
	void GetWorldRay(IDirect3DDevice9* pDevice, long x, long y, long width, long height, Nav::Vector3& orig, Nav::Vector3& dir);

	bool IsTriangleInSameMesh(Nav::NavTriangle* tri1, Nav::NavTriangle* tri2, Nav::NavGraph*& outFinder);

	void TransformPos(Nav::Vector3& pos);

	void OnMenu();

	void OnInput();

	void OpenFBX(const char* filePath);

	void OpenNav(const char* filePath);

	void CloseFile();
public:
	MeshRenderer* mRenderer;
	// UILayout
	float mLeftUIWidth;

	Nav::NavSystem* mNavSystem;

	float mDeltaTime;
private:
	IDirect3DDevice9* mDevice;
	HWND mHwnd;
	int mWidth;
	int mHeight;

	PathFindLogic* mPathFindLogic;
	GateLogic* mGateLogic;
	CheckInfoLogic* mCheckInfoLogic;
	GraphEditLogic* mGraphEditLogic;

	// FileDialog
	FileDialog* mOpenFBX;
	FileDialog* mOpenNav;
	FileDialog* mSaveNav;

	// Camera
	Camera* mCamera;
	float mModelSize;

	static Test* mInstance;
};

#endif