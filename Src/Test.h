#ifndef __TEST_H__
#define __TEST_H__

#include "inc.h"

class MeshRenderer;

class Test
{
public:
	Test();
	~Test();

	void OnInit(HWND hwnd, IDirect3DDevice9* device);

	void OnUpdate();

	void OnGUI();

	void OnQuit();
public:
	IDirect3DDevice9* mDevice;
	HWND mHwnd;
	int mWidth;
	int mHeight;
	MeshRenderer* mRenderer;
	D3DMATERIAL9 mMatNav;
};

#endif