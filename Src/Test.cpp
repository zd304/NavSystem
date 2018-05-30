#include "Test.h"
#include "imgui/imgui.h"
#include "MeshRenderer.h"

Test::Test()
{
	mRenderer = NULL;
}

Test::~Test()
{
	SAFE_DELETE(mRenderer);
}

void LoadTextures(IDirect3DDevice9* device)
{
	LPDIRECT3DTEXTURE9 p = NULL;
	D3DXCreateTextureFromFileEx(device, "./Image/disk.png",
		D3DX_FROM_FILE,
		D3DX_FROM_FILE,
		D3DX_DEFAULT,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE,
		D3DX_FILTER_TRIANGLE,
		D3DCOLOR_RGBA(0, 0, 0, 255),
		NULL,
		NULL,
		&p);
	Global::mDiskTexID = (ImTextureID)p;
	p = NULL;
	D3DXCreateTextureFromFileEx(device, "./Image/folder.png",
		D3DX_FROM_FILE,
		D3DX_FROM_FILE,
		D3DX_DEFAULT,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE,
		D3DX_FILTER_TRIANGLE,
		D3DCOLOR_RGBA(0, 0, 0, 255),
		NULL,
		NULL,
		&p);
	Global::mFolderTexID = (ImTextureID)p;
	p = NULL;
	D3DXCreateTextureFromFileEx(device, "./Image/file.png",
		D3DX_FROM_FILE,
		D3DX_FROM_FILE,
		D3DX_DEFAULT,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE,
		D3DX_FILTER_TRIANGLE,
		D3DCOLOR_RGBA(0, 0, 0, 255),
		NULL,
		NULL,
		&p);
	Global::mFileTexID = (ImTextureID)p;
}

void Test::OnInit(HWND hwnd, IDirect3DDevice9* device)
{
	mDevice = device;
	mHwnd = hwnd;
	RECT rc;
	::GetClientRect(hwnd, &rc);
	mWidth = (int)(rc.right - rc.left);
	mHeight = (int)(rc.bottom - rc.top);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("./Font/msyh.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChinese());
	LoadTextures(device);

	FBXHelper::BeginFBXHelper("./Res/nav.FBX");

	D3DXMATRIX matProj, mMatWorld;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)mWidth / (float)mHeight, 0.1f, 10000.0f);
	D3DXMatrixScaling(&mMatWorld, 1.0f, 1.0f, 1.0f);

	D3DXVECTOR3 max, min;
	FBXHelper::GetBox(max, min);
	float boxSize = D3DXVec3Length(&(max - min));
	float mCameraDistance = 2.0f * boxSize;
	float mCameraHeight = 2.0f * boxSize;
	float mCameraX = 0.0f;// -boxSize;
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(mCameraX, mCameraHeight, mCameraDistance),
		&D3DXVECTOR3(mCameraX, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	mDevice->SetTransform(D3DTS_VIEW, &matView);
	device->SetTransform(D3DTS_PROJECTION, &matProj);
	device->SetTransform(D3DTS_WORLD, &mMatWorld);

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	mRenderer = new MeshRenderer(mDevice, FBXHelper::GetMeshDatas());

	FBXHelper::EndFBXHelper();

	mMatNav.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
}

void Test::OnGUI()
{
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(256, (float)mHeight));
	ImGui::Begin(STU("µ¼º½±à¼­Æ÷").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

	

	ImGui::End();

	//ImGui::PopStyleColor();
}

void Test::OnUpdate()
{
	if (mRenderer)
	{
		mDevice->SetMaterial(&mMatNav);

		mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		mRenderer->Render();
		mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		//mRenderer->Render();
	}
}

void Test::OnQuit()
{
}