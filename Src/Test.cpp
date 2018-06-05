#include "Test.h"
#include "imgui/imgui.h"
#include "MeshRenderer.h"
#include "NavMesh.h"
#include "NavTriangle.h"
#include "NavPhysics.h"
#include "NavGraph.h"
#include "PathFindLogic.h"

struct SelectedMeshVertex
{
	Vector3 pos;
	DWORD color;

	static const int fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

Test::Test()
{
	mRenderer = NULL;
	mPathFindLogic = NULL;
}

Test::~Test()
{
	for (size_t i = 0; i < mNavGraphs.size(); ++i)
	{
		NavGraph* mesh = mNavGraphs[i];
		SAFE_DELETE(mesh);
	}
	mNavGraphs.clear();
	SAFE_DELETE(mRenderer);
	SAFE_DELETE(mPathFindLogic);
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

	
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)mWidth / (float)mHeight, 0.1f, 10000.0f);
	device->SetTransform(D3DTS_PROJECTION, &matProj);

	rot = 0.0f;

	D3DXVECTOR3 max, min;
	FBXHelper::GetBox(max, min);
	float boxSize = D3DXVec3Length(&(max - min));
	mCameraDistance = 2.0f * boxSize;
	mCameraHeight = 2.0f * boxSize;
	mCameraX = 0.0f;// -boxSize;

	UpdateView();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	FBXHelper::FBXMeshDatas* meshDatas = FBXHelper::GetMeshDatas();
	mRenderer = new MeshRenderer(mDevice, meshDatas);
	mPathFindLogic = new PathFindLogic(mRenderer);
	for (size_t i = 0; i < meshDatas->datas.size(); ++i)
	{
		FBXHelper::FBXMeshData* data = meshDatas->datas[i];
		NavMesh* navMesh = new NavMesh((Vector3*)&data->pos[0], data->pos.size(), &data->indices[0], data->indices.size());
		NavGraph* pathFinder = new NavGraph(navMesh);
		mNavGraphs.push_back(pathFinder);
	}

	FBXHelper::EndFBXHelper();
}

void Test::UpdateView()
{
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(-mCameraX, mCameraHeight, mCameraDistance),
		&D3DXVECTOR3(-mCameraX, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	mDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixRotationYawPitchRoll(&mWorldMtrix, -rot, 0.0f, 0.0f);
	mDevice->SetTransform(D3DTS_WORLD, &mWorldMtrix);
}

void Test::OnGUI()
{
	//ImGui::ShowTestWindow();

	if (ImGui::IsKeyPressed('A'))
	{
		mCameraX -= (mCameraDistance * 0.01f);
		UpdateView();
	}
	if (ImGui::IsKeyPressed('D'))
	{
		mCameraX += (mCameraDistance * 0.01f);
		UpdateView();
	}
	if (ImGui::IsKeyPressed('W'))
	{
		mCameraDistance -= (mCameraDistance * 0.01f);
		UpdateView();
	}
	if (ImGui::IsKeyPressed('S'))
	{
		mCameraDistance += (mCameraDistance * 0.01f);
		UpdateView();
	}
	ImGuiIO& io = ImGui::GetIO();
	if (fabs(io.MouseWheel) > FLT_EPSILON)
	{
		mCameraHeight += (io.MouseWheel * (mCameraDistance + 1.0f) * 0.1f);
		UpdateView();
	}
	if (ImGui::IsMouseDragging(0))
	{
		ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
		rot -= dragDelta.x * 0.01f;
		UpdateView();

		ImGui::ResetMouseDragDelta(0);
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(256, (float)mHeight));
	ImGui::Begin(STU("�����༭��").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

	ImGui::End();
}

void Test::OnUpdate()
{
	if (mRenderer)
	{
		mRenderer->Render();
	}
}

void Test::OnQuit()
{
}

bool Test::IsTriangleInSameMesh(NavTriangle* tri1, NavTriangle* tri2, NavGraph*& outFinder)
{
	for (size_t i = 0; i < mNavGraphs.size(); ++i)
	{
		bool exist1 = false;
		bool exist2 = false;
		NavGraph* finder = mNavGraphs[i];
		for (size_t j = 0; j < finder->mMesh->mTriangles.size(); ++j)
		{
			NavTriangle* tri = finder->mMesh->mTriangles[j];
			if (!exist1 && tri1 == tri)
				exist1 = true;
			if (!exist2 && tri2 == tri)
				exist2 = true;
			if (exist1 && exist2)
			{
				outFinder = finder;
				return true;
			}
		}
	}
	return false;
}

void Test::GetWorldRay(IDirect3DDevice9* pDevice, long x, long y, long width, long height, Vector3& orig, Vector3& dir)
{
	D3DXMATRIX matView;
	D3DXMATRIX matProj;

	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXVECTOR3 vTemp;
	vTemp.x = (((2.0f * (float)x) / (float)width) - 1.0f) / matProj._11;
	vTemp.y = (((-2.0f * (float)y) / (float)height) + 1.0f) / matProj._22;
	vTemp.z = 1.0f;
	D3DXVECTOR3 vPickRayDir(vTemp.x, vTemp.y, 1.0f);
	D3DXVECTOR3 vPickRayOrig(0.0f, 0.0f, 0.0f);

	D3DXMATRIX matInv = matView;
	D3DXMatrixInverse(&matInv, NULL, &matInv);
	D3DXVec3TransformCoord(&vPickRayOrig, &vPickRayOrig, &matInv);
	D3DXVec3TransformNormal(&vPickRayDir, &vPickRayDir, &matInv);
	D3DXVec3Normalize(&vPickRayDir, &vPickRayDir);

	orig.Set(vPickRayOrig.x, vPickRayOrig.y, vPickRayOrig.z);
	dir.Set(vPickRayDir.x, vPickRayDir.y, vPickRayDir.z);
}

void Test::Pick(int x, int y)
{
	RECT rect;
	GetClientRect(mHwnd, &rect);
	long w = rect.right - rect.left;
	long h = rect.bottom - rect.top;

	Vector3 orig;
	Vector3 dir;
	GetWorldRay(mDevice, x, y, w, h, orig, dir);

	NavTriangle* hitTri = NULL;
	NavGraph* hitGraph = NULL;
	Vector3 hitPoint = Vector3::ZERO;

	for (size_t i = 0; i < mNavGraphs.size(); ++i)
	{
		NavGraph* navPathFinder = mNavGraphs[i];
		NavMesh* navMesh = navPathFinder->mMesh;
		for (size_t j = 0; j < navMesh->mTriangles.size(); ++j)
		{
			NavTriangle* tri = navMesh->mTriangles[j];
			NavPhysics::NavHit hitInfo;

			Vector3 v0 = tri->mPoint[0];
			Vector3 v1 = tri->mPoint[1];
			Vector3 v2 = tri->mPoint[2];
			TransformPos(v0); TransformPos(v1); TransformPos(v2);
			if (!NavPhysics::RayIntersectTriangle(orig, dir, v0, v1, v2, &hitInfo))
				continue;
			hitPoint = hitInfo.hitPoint;
			hitTri = tri;
			hitGraph = navPathFinder;
			break;
		}
		if (hitGraph != NULL)
			break;
	}

	if (mPathFindLogic)
		mPathFindLogic->OnPick(hitTri, hitPoint, hitGraph);
}

void Test::TransformPos(Vector3& pos)
{
	D3DXVECTOR3 v(pos.x, pos.y, pos.z);
	D3DXVec3TransformCoord(&v, &v, &mWorldMtrix);
	pos.Set(v.x, v.y, v.z);
}