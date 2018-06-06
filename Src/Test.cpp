#include "Test.h"
#include "imgui/imgui.h"
#include "MeshRenderer.h"
#include "NavMesh.h"
#include "NavTriangle.h"
#include "NavPhysics.h"
#include "NavGraph.h"
#include "NavSystem.h"
#include "FileDialog.h"
#include "PathFindLogic.h"

struct SelectedMeshVertex
{
	Vector3 pos;
	DWORD color;

	static const int fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

Test* Test::mInstance = NULL;
Test* Test::GetInstance()
{
	return mInstance;
}

Test::Test()
{
	mRenderer = NULL;
	mNavSystem = new NavSystem();
	mPathFindLogic = NULL;
	mInstance = this;

	char szPath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szPath);
	std::string sCurPath = szPath;
	sCurPath = sCurPath.append("\\");

	mOpenFBX = new FileDialog("打开FBX", "fbx", eFileDialogUsage_OpenFile);
	mOpenFBX->SetDefaultDirectory(sCurPath);
	mSaveNav = new FileDialog("保存导航文件", "nav", eFileDialogUsage_SaveFile);
	mSaveNav->SetDefaultDirectory(sCurPath);
	mOpenNav = new FileDialog("打开导航", "nav", eFileDialogUsage_OpenFile);
	mOpenNav->SetDefaultDirectory(sCurPath);
}

Test::~Test()
{
	SAFE_DELETE(mNavSystem);
	SAFE_DELETE(mRenderer);
	SAFE_DELETE(mPathFindLogic);
	SAFE_DELETE(mOpenFBX);
	SAFE_DELETE(mSaveNav);
	SAFE_DELETE(mOpenNav);
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

	rot = 0.0f;
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, (float)mWidth / (float)mHeight, 0.1f, 10000.0f);
	device->SetTransform(D3DTS_PROJECTION, &matProj);

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
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

	OnInput();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(256, (float)mHeight));
	ImGui::Begin(STU("导航编辑器").c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

	OnMenu();

	if (mOpenFBX->DoModal())
	{
		std::string path;
		path += mOpenFBX->GetDirectory();
		path += mOpenFBX->GetFileName();
		OpenFBX(path.c_str());
	}
	if (mSaveNav->DoModal())
	{
		std::string path;
		path += mSaveNav->GetDirectory();
		path += mSaveNav->GetFileName();
		mNavSystem->SaveAs(path.c_str());
	}
	if (mOpenNav->DoModal())
	{
		std::string path;
		path += mOpenNav->GetDirectory();
		path += mOpenNav->GetFileName();

		OpenNav(path.c_str());
	}

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

void Test::OnMenu()
{
	bool openFlag = false;
	bool open1Flag = false;
	bool saveFlag = false;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(STU("文件").c_str()))
		{
			if (ImGui::MenuItem(STU("导入模型").c_str(), NULL))
			{
				openFlag = true;
			}
			if (ImGui::MenuItem(STU("打开导航").c_str(), NULL))
			{
				open1Flag = true;
			}
			if (ImGui::MenuItem(STU("保存导航").c_str(), NULL))
			{
				saveFlag = true;
			}
			if (ImGui::MenuItem(STU("清除导航").c_str(), NULL))
			{
				CloseFile();
			}
			ImGui::Separator();
			if (ImGui::MenuItem(STU("关闭").c_str(), NULL))
			{
				::PostQuitMessage(0);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(STU("模型").c_str()))
		{
			if (ImGui::MenuItem(STU("显示导航模型").c_str(), NULL, &mRenderer->mShowNavMesh))
			{
			}
			if (ImGui::MenuItem(STU("显示高度图").c_str(), NULL, &mRenderer->mShowHeightmap))
			{
			}
			if (ImGui::MenuItem(STU("显示导航网格").c_str(), NULL, &mRenderer->mShowWireMesh))
			{
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (openFlag)
	{
		mOpenFBX->Open();
		openFlag = false;
	}
	if (open1Flag)
	{
		mOpenNav->Open();
		open1Flag = false;
	}
	if (saveFlag)
	{
		mSaveNav->Open();
		saveFlag = false;
	}
}

void Test::OnInput()
{
	if (mOpenFBX && mOpenFBX->IsOpen())
		return;

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
}

void Test::OpenFBX(const char* filePath)
{
	CloseFile();

	FBXHelper::BeginFBXHelper(filePath);

	D3DXVECTOR3 max, min;
	FBXHelper::GetBox(max, min);
	float boxSize = D3DXVec3Length(&(max - min));
	mCameraDistance = 2.0f * boxSize;
	mCameraHeight = 2.0f * boxSize;
	mCameraX = 0.0f;// -boxSize;

	UpdateView();

	FBXHelper::FBXMeshDatas* meshDatas = FBXHelper::GetMeshDatas();

	mRenderer = new MeshRenderer(mDevice, meshDatas);
	mPathFindLogic = new PathFindLogic(this);

	for (size_t i = 0; i < meshDatas->datas.size(); ++i)
	{
		FBXHelper::FBXMeshData* data = meshDatas->datas[i];
		NavMesh* navMesh = new NavMesh((Vector3*)&data->pos[0], data->pos.size(), &data->indices[0], data->indices.size());
		NavGraph* pathFinder = new NavGraph(navMesh);
		mNavSystem->AddGraph(pathFinder);

		mRenderer->SetHeightmap(pathFinder->mHeightmap, i);
	}

	FBXHelper::EndFBXHelper();
}

void Test::OpenNav(const char* filePath)
{
	CloseFile();

	mNavSystem->LoadFromFile(filePath);

	FBXHelper::InitBox();

	FBXHelper::FBXMeshDatas datas;
	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		NavGraph* graph = mNavSystem->GetGraphByID(i);
		FBXHelper::FBXMeshData* data = new FBXHelper::FBXMeshData();
		int index = -1;
		for (size_t j = 0; j < graph->mMesh->mTriangles.size(); ++j)
		{
			NavTriangle* tri = graph->mMesh->mTriangles[j];
			D3DXVECTOR3 v0 = *(D3DXVECTOR3*)&tri->mPoint[0];
			D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&tri->mPoint[1];
			D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&tri->mPoint[2];
			FBXHelper::UpdateBox(v0);
			FBXHelper::UpdateBox(v1);
			FBXHelper::UpdateBox(v2);
			data->pos.push_back(v0);
			data->pos.push_back(v1);
			data->pos.push_back(v2);

			data->indices.push_back(++index);
			data->indices.push_back(++index);
			data->indices.push_back(++index);
		}
		datas.datas.push_back(data);
	}
	mRenderer = new MeshRenderer(mDevice, &datas);
	mPathFindLogic = new PathFindLogic(this);

	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		NavGraph* graph = mNavSystem->GetGraphByID(i);
		mRenderer->SetHeightmap(graph->mHeightmap, i);
	}

	D3DXVECTOR3 max, min;
	FBXHelper::GetBox(max, min);
	float boxSize = D3DXVec3Length(&(max - min));
	mCameraDistance = 2.0f * boxSize;
	mCameraHeight = 2.0f * boxSize;
	mCameraX = 0.0f;// -boxSize;

	UpdateView();
}

void Test::CloseFile()
{
	mNavSystem->Clear();
	SAFE_DELETE(mRenderer);
	SAFE_DELETE(mPathFindLogic);
}

bool Test::IsTriangleInSameMesh(NavTriangle* tri1, NavTriangle* tri2, NavGraph*& outFinder)
{
	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		bool exist1 = false;
		bool exist2 = false;
		NavGraph* finder = mNavSystem->GetGraphByID(i);
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

	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		NavGraph* navPathFinder = mNavSystem->GetGraphByID(i);
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