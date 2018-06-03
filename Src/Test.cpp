#include "Test.h"
#include "imgui/imgui.h"
#include "MeshRenderer.h"
#include "NavMesh.h"
#include "NavTriangle.h"
#include "NavPhysics.h"
#include "NavPathFinder.h"

struct SelectedMeshVertex
{
	Vector3 pos;
	DWORD color;

	static const int fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

Test::Test()
{
	mRenderer = NULL;
	mSelectedMesh = NULL;
	mStartTri = NULL;
	mEndTri = NULL;
	mStartMesh = NULL;
	mEndMesh = NULL;
	mClickMode = eClickMode::None;
}

Test::~Test()
{
	for (size_t i = 0; i < mNavMeshes.size(); ++i)
	{
		NavPathFinder* mesh = mNavMeshes[i];
		SAFE_DELETE(mesh);
	}
	mNavMeshes.clear();
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
	D3DXMatrixIdentity(&mMatWorld);

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

	FBXHelper::FBXMeshDatas* meshDatas = FBXHelper::GetMeshDatas();
	mRenderer = new MeshRenderer(mDevice, meshDatas);
	for (size_t i = 0; i < meshDatas->datas.size(); ++i)
	{
		FBXHelper::FBXMeshData* data = meshDatas->datas[i];
		NavMesh* navMesh = new NavMesh((Vector3*)&data->pos[0], data->pos.size(), &data->indices[0], data->indices.size());
		NavPathFinder* pathFinder = new NavPathFinder(navMesh);
		mNavMeshes.push_back(pathFinder);
	}

	FBXHelper::EndFBXHelper();

	mMatNav.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
}

void Test::OnGUI()
{
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(256, (float)mHeight));
	ImGui::Begin(STU("导航编辑器").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

	if (mSelectedTriangles.size() > 0)
	{
		if (ImGui::Button(STU("清空").c_str()))
		{
			ClearPath();
		}
	}
	else
	{
		eClickMode e = mClickMode;
		if (e == eClickMode::Start)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
		}
		if (ImGui::Button(STU("选择起始点").c_str()))
		{
			if (mClickMode == eClickMode::Start)
				mClickMode = eClickMode::None;
			else
				mClickMode = eClickMode::Start;
		}
		if (e == eClickMode::Start)
		{
			ImGui::PopStyleColor(2);
		}

		if (e == eClickMode::End)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
		}
		if (ImGui::Button(STU("选择结束点").c_str()))
		{
			if (mClickMode == eClickMode::End)
				mClickMode = eClickMode::None;
			else
				mClickMode = eClickMode::End;
		}
		if (e == eClickMode::End)
		{
			ImGui::PopStyleColor(2);
		}
		if (ImGui::Button(STU("生成路径").c_str()))
		{
			NavPathFinder* finder = NULL;
			if (mStartTri != NULL && mEndTri != NULL && IsTriangleInSameMesh(mStartTri, mEndTri, finder))
			{
				std::vector<NavTriangle*> findPath;
				float cost;
				if (finder->Solve(mStartTri, mEndTri, &findPath, &cost))
				{
					for (size_t i = 0; i < findPath.size(); ++i)
					{
						NavTriangle* tri = findPath[i];
						AddSelectedTriangle(tri);
					}
				}
			}
		}
	}

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
		if (mSelectedMesh)
			mSelectedMesh->DrawSubset(0);
		if (mStartMesh)
			mStartMesh->DrawSubset(0);
		if (mEndMesh)
			mEndMesh->DrawSubset(0);
	}
}

void Test::OnQuit()
{
}

bool Test::IsTriangleInSameMesh(NavTriangle* tri1, NavTriangle* tri2, NavPathFinder*& outFinder)
{
	for (size_t i = 0; i < mNavMeshes.size(); ++i)
	{
		bool exist1 = false;
		bool exist2 = false;
		NavPathFinder* finder = mNavMeshes[i];
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
	for (size_t i = 0; i < mNavMeshes.size(); ++i)
	{
		NavPathFinder* navPathFinder = mNavMeshes[i];
		NavMesh* navMesh = navPathFinder->mMesh;
		for (size_t j = 0; j < navMesh->mTriangles.size(); ++j)
		{
			NavTriangle* tri = navMesh->mTriangles[j];
			NavPhysics::NavHit hitInfo;
			if (!NavPhysics::RayIntersectTriangle(orig, dir, tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], &hitInfo))
				continue;
			//AddSelectedTriangle(tri);
			if (mClickMode == Start)
			{
				SetPointMesh(tri, true);
				return;
			}
			else if (mClickMode == End)
			{
				SetPointMesh(tri, false);
				return;
			}
			break;
		}
	}
}

void Test::AddSelectedTriangle(NavTriangle* tri)
{
	SAFE_RELEASE(mSelectedMesh);

	mSelectedTriangles.push_back(tri);
	size_t count = mSelectedTriangles.size() * 3;
	HRESULT hr = D3DXCreateMeshFVF(mSelectedTriangles.size(), count, D3DXMESH_32BIT, SelectedMeshVertex::fvf, mDevice, &mSelectedMesh);
	if (FAILED(hr))
		return;

	SelectedMeshVertex* vb = NULL;
	mSelectedMesh->LockVertexBuffer(NULL, (void**)&vb);
	for (size_t i = 0; i < mSelectedTriangles.size(); ++i)
	{
		NavTriangle* t = mSelectedTriangles[i];
		SelectedMeshVertex& v0 = vb[i * 3];
		v0.color = 0xffff0000;
		v0.pos = t->mPoint[0];
		SelectedMeshVertex& v1 = vb[i * 3 + 1];
		v1.color = 0xffff0000;
		v1.pos = t->mPoint[1];
		SelectedMeshVertex& v2 = vb[i * 3 + 2];
		v2.color = 0xffff0000;
		v2.pos = t->mPoint[2];
	}
	mSelectedMesh->UnlockVertexBuffer();

	unsigned int* ib = NULL;
	mSelectedMesh->LockIndexBuffer(NULL, (void**)&ib);
	for (size_t i = 0; i < mSelectedTriangles.size(); ++i)
	{
		ib[i * 3] = i * 3;
		ib[i * 3 + 1] = i * 3 + 1;
		ib[i * 3 + 2] = i * 3 + 2;
	}
	mSelectedMesh->UnlockIndexBuffer();
}

void Test::SetPointMesh(NavTriangle* tri, bool isStart)
{
	ID3DXMesh** ppMesh = NULL;
	DWORD color = 0;
	if (isStart)
	{
		SAFE_RELEASE(mStartMesh);
		mStartTri = tri;
		ppMesh = &mStartMesh;
		color = 0xffffff00;
	}
	else
	{
		SAFE_RELEASE(mEndMesh);
		mEndTri = tri;
		ppMesh = &mEndMesh;
		color = 0xff0000ff;
	}
	HRESULT hr = D3DXCreateMeshFVF(1, 3, D3DXMESH_32BIT, SelectedMeshVertex::fvf, mDevice, ppMesh);
	if (FAILED(hr))
		return;
	SelectedMeshVertex* vb = NULL;
	(*ppMesh)->LockVertexBuffer(NULL, (void**)&vb);
	{
		SelectedMeshVertex& v0 = vb[0];
		v0.color = color;
		v0.pos = tri->mPoint[0];
		SelectedMeshVertex& v1 = vb[1];
		v1.color = color;
		v1.pos = tri->mPoint[1];
		SelectedMeshVertex& v2 = vb[2];
		v2.color = color;
		v2.pos = tri->mPoint[2];
	}
	(*ppMesh)->UnlockVertexBuffer();

	unsigned int* ib = NULL;
	(*ppMesh)->LockIndexBuffer(NULL, (void**)&ib);
	{
		ib[0] = 0;
		ib[1] = 1;
		ib[2] = 2;
	}
	(*ppMesh)->UnlockIndexBuffer();
}

void Test::ClearPath()
{
	SAFE_RELEASE(mStartMesh);
	mStartTri = NULL;
	SAFE_RELEASE(mEndMesh);
	mEndTri = NULL;
	SAFE_RELEASE(mSelectedMesh);
	mSelectedTriangles.clear();
	mClickMode = None;
}