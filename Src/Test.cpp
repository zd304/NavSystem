#include "Test.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "NavMesh.h"
#include "NavTriangle.h"
#include "NavPhysics.h"
#include "NavGraph.h"
#include "NavSystem.h"
#include "FileDialog.h"
#include "PathFindLogic.h"
#include "GateLogic.h"
#include "CheckInfoLogic.h"
#include "GraphEditLogic.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

struct SelectedMeshVertex
{
	Nav::Vector3 pos;
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
	mNavSystem = new Nav::NavSystem();
	mPathFindLogic = NULL;
	mGateLogic = NULL;
	mCheckInfoLogic = NULL;
	mGraphEditLogic = NULL;

	mCamera = NULL;
	mInstance = this;
	mModelSize = 0.0f;
	mDeltaTime = 0;

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

	mLeftUIWidth = 256;
	mModelSize = 0.0f;
}

Test::~Test()
{
	SAFE_DELETE(mCamera);
	SAFE_DELETE(mNavSystem);
	SAFE_DELETE(mRenderer);
	SAFE_DELETE(mPathFindLogic);
	SAFE_DELETE(mGateLogic);
	SAFE_DELETE(mCheckInfoLogic);
	SAFE_DELETE(mGraphEditLogic);
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

	mCamera = new Camera(Nav::Vector3::ZERO, Nav::Vector3(0.0f, 0.0f, 1.0f),
		D3DX_PI / 4.0f, (float)mWidth / (float)mHeight, 0.1f, 10000.0f);

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void Test::OnGUI()
{
	//ImGui::ShowTestWindow();

	OnInput();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(mLeftUIWidth, (float)mHeight));
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

	if (!mGateLogic && !mPathFindLogic && !mCheckInfoLogic && !mGraphEditLogic)
	{
		if (ImGui::Button(STU("寻路模式").c_str(), ImVec2(mLeftUIWidth - 20.0f, 30.0f)))
		{
			mPathFindLogic = new PathFindLogic(this);
		}
		if (ImGui::Button(STU("门编辑模式").c_str(), ImVec2(mLeftUIWidth - 20.0f, 30.0f)))
		{
			mGateLogic = new GateLogic(this);
		}
		if (ImGui::Button(STU("三角形模式").c_str(), ImVec2(mLeftUIWidth - 20.0f, 30.0f)))
		{
			mCheckInfoLogic = new CheckInfoLogic(this);
		}
		if (ImGui::Button(STU("ID编辑模式").c_str(), ImVec2(mLeftUIWidth - 20.0f, 30.0f)))
		{
			mGraphEditLogic = new GraphEditLogic(this);
		}
	}
	else
	{
		if (ImGui::Button(STU("退出编辑模式").c_str(), ImVec2(mLeftUIWidth - 20.0f, 30.0f)))
		{
			SAFE_DELETE(mPathFindLogic);
			SAFE_DELETE(mGateLogic);
			SAFE_DELETE(mCheckInfoLogic);
			SAFE_DELETE(mGraphEditLogic);
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	if (mGateLogic)
	{
		mGateLogic->OnGUI();
	}
	if (mPathFindLogic)
	{
		mPathFindLogic->OnGUI();
	}
	if (mCheckInfoLogic)
	{
		mCheckInfoLogic->OnGUI();
	}
	if (mGraphEditLogic)
	{
		mGraphEditLogic->OnGUI();
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
		if (ImGui::BeginMenu(STU("显示").c_str()))
		{
			if (mRenderer)
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
			}			
			if (mPathFindLogic)
			{
				if (ImGui::MenuItem(STU("显示三角形路径").c_str(), NULL, &mPathFindLogic->mShowTriPath))
				{
				}
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
	if (!mCamera)
		return;

	float delta = (float)mDeltaTime;

	Nav::Vector3 pos = mCamera->GetPosition();
	Nav::Vector3 forward = mCamera->GetDir();

	if (ImGui::IsKeyPressed('A'))
	{
		Nav::Vector3 right = mCamera->GetRight();
		mCamera->SetPosition(pos - right * pos.Length() * delta * 30.0f);
		mCamera->Update(mDevice);
	}
	if (ImGui::IsKeyPressed('D'))
	{
		Nav::Vector3 right = mCamera->GetRight();
		mCamera->SetPosition(pos + right * pos.Length() * delta * 30.0f);
		mCamera->Update(mDevice);
	}
	if (ImGui::IsKeyPressed('W'))
	{
		mCamera->SetPosition(pos + forward * pos.Length() * delta * 30.0f);
		mCamera->Update(mDevice);
	}
	if (ImGui::IsKeyPressed('S'))
	{
		mCamera->SetPosition(pos - forward * pos.Length() * delta * 30.0f);
		mCamera->Update(mDevice);
	}
	if (ImGui::IsKeyReleased('F'))
	{
		mCamera->SetPosition(Nav::Vector3(0.0f, 2.0f * mModelSize, -2.0f * mModelSize));
		mCamera->SetForward(Nav::Vector3(0.0f, -1.0f, 1.0f));
		mCamera->Update(mDevice);
	}
	ImGuiIO& io = ImGui::GetIO();
	if (fabs(io.MouseWheel) > FLT_EPSILON)
	{
		mCamera->SetPosition(pos + io.MouseWheel * forward * pos.Length() * delta * 200.0f);
		mCamera->Update(mDevice);
	}

	if (ImGui::IsMouseDragging(1))
	{
		ImVec2 dragDelta = ImGui::GetMouseDragDelta(1);

		Nav::Vector3 right = mCamera->GetRight();

		D3DXMATRIX rot, rotYaw, rotPitch;
		D3DXVECTOR3 vR(right.x, right.y, right.z);
		D3DXMatrixRotationAxis(&rotYaw, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), dragDelta.x * delta * 8.0f);
		D3DXMatrixRotationAxis(&rotPitch, &vR, dragDelta.y * delta * 8.0f);

		D3DXVECTOR3 dir(forward.x, forward.y, forward.z);
		rot = rotYaw * rotPitch;
		D3DXVec3TransformNormal(&dir, &dir, &rot);
		forward.Set(dir.x, dir.y, dir.z);
		mCamera->SetForward(forward);
		mCamera->Update(mDevice);

		ImGui::ResetMouseDragDelta(1);
	}
}

bool CompareTriangleArea(const Nav::NavTriangle* t1, const Nav::NavTriangle* t2)
{
	float area1 = NavPhysics::CalcTriangleArea2D(t1->mPoint[0], t1->mPoint[1], t1->mPoint[2]);
	float area2 = NavPhysics::CalcTriangleArea2D(t2->mPoint[0], t2->mPoint[1], t2->mPoint[2]);

	if (area1 > area2)
	{
		return true;
	}
	return false;
}

void Test::OpenFBX(const char* filePath)
{
	CloseFile();

	FBXHelper::BeginFBXHelper(filePath);

	D3DXVECTOR3 max, min;
	FBXHelper::GetBox(max, min);
	mModelSize = D3DXVec3Length(&(max - min));

	if (mCamera)
	{
		mCamera->SetPosition(Nav::Vector3(0.0f, 2.0f * mModelSize, -2.0f * mModelSize));
		mCamera->SetForward(Nav::Vector3(0.0f, -1.0f, 1.0f));
		mCamera->Update(mDevice);
	}

	FBXHelper::FBXMeshDatas* meshDatas = FBXHelper::GetMeshDatas();

	mRenderer = new MeshRenderer(mDevice, meshDatas, this);

	for (size_t i = 0; i < meshDatas->datas.size(); ++i)
	{
		FBXHelper::FBXMeshData* data = meshDatas->datas[i];
		Nav::NavMesh* navMesh = new Nav::NavMesh((Nav::Vector3*)&data->pos[0], data->pos.size(), &data->indices[0], data->indices.size());
		Nav::NavGraph* pathFinder = new Nav::NavGraph(navMesh);

		//三角形按面积大小排序;
		std::sort(navMesh->mTriangles.begin(), navMesh->mTriangles.end(), CompareTriangleArea);

		mNavSystem->AddGraph(pathFinder);

		mRenderer->SetHeightmap(pathFinder->mHeightmap, i);
	}
	mRenderer->CalcAllCloseGates();

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
		Nav::NavGraph* graph = mNavSystem->GetGraphByIndex(i);
		FBXHelper::FBXMeshData* data = new FBXHelper::FBXMeshData();
		int index = -1;
		for (size_t j = 0; j < graph->mMesh->mTriangles.size(); ++j)
		{
			Nav::NavTriangle* tri = graph->mMesh->mTriangles[j];
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
	mRenderer = new MeshRenderer(mDevice, &datas, this);
	//mPathFindLogic = new PathFindLogic(this);

	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		Nav::NavGraph* graph = mNavSystem->GetGraphByIndex(i);
		mRenderer->SetHeightmap(graph->mHeightmap, i);
	}
	mRenderer->CalcAllCloseGates();

	D3DXVECTOR3 max, min;
	FBXHelper::GetBox(max, min);
	mModelSize = D3DXVec3Length(&(max - min));

	if (mCamera)
	{
		mCamera->SetPosition(Nav::Vector3(0.0f, 2.0f * mModelSize, -2.0f * mModelSize));
		mCamera->SetForward(Nav::Vector3(0.0f, -1.0f, 1.0f));
		mCamera->Update(mDevice);
	}
}

void Test::CloseFile()
{
	mNavSystem->Clear();
	SAFE_DELETE(mRenderer);
	SAFE_DELETE(mPathFindLogic);
	SAFE_DELETE(mGateLogic);
	SAFE_DELETE(mCheckInfoLogic);
}

bool Test::IsTriangleInSameMesh(Nav::NavTriangle* tri1, Nav::NavTriangle* tri2, Nav::NavGraph*& outFinder)
{
	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		bool exist1 = false;
		bool exist2 = false;
		Nav::NavGraph* finder = mNavSystem->GetGraphByIndex(i);
		for (size_t j = 0; j < finder->mMesh->mTriangles.size(); ++j)
		{
			Nav::NavTriangle* tri = finder->mMesh->mTriangles[j];
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

void Test::GetWorldRay(IDirect3DDevice9* pDevice, long x, long y, long width, long height, Nav::Vector3& orig, Nav::Vector3& dir)
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

	Nav::Vector3 orig;
	Nav::Vector3 dir;
	GetWorldRay(mDevice, x, y, w, h, orig, dir);
	
	D3DXMATRIX worldInv;
	D3DXMatrixIdentity(&worldInv); //D3DXMatrixInverse(&worldInv, NULL, &mWorldMtrix);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&orig, (D3DXVECTOR3*)&orig, &worldInv);
	D3DXVec3TransformNormal((D3DXVECTOR3*)&dir, (D3DXVECTOR3*)&dir, &worldInv);

	Nav::NavTriangle* hitTri = NULL;
	Nav::NavGraph* hitGraph = NULL;
	Nav::Vector3 hitPoint = Nav::Vector3::ZERO;

	for (size_t i = 0; i < mNavSystem->GetGraphCount(); ++i)
	{
		Nav::NavGraph* navPathFinder = mNavSystem->GetGraphByIndex(i);
		Nav::NavMesh* navMesh = navPathFinder->mMesh;
		for (size_t j = 0; j < navMesh->mTriangles.size(); ++j)
		{
			Nav::NavTriangle* tri = navMesh->mTriangles[j];
			NavPhysics::NavHit hitInfo;

			Nav::Vector3 v0 = tri->mPoint[0];
			Nav::Vector3 v1 = tri->mPoint[1];
			Nav::Vector3 v2 = tri->mPoint[2];
			//TransformPos(v0); TransformPos(v1); TransformPos(v2);
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
	if (mGateLogic)
		mGateLogic->OnPick(hitTri, hitPoint, hitGraph);
	if (mCheckInfoLogic)
		mCheckInfoLogic->OnPick(hitTri, hitPoint, hitGraph);
	if (mGraphEditLogic)
		mGraphEditLogic->OnPick(hitTri, hitPoint, hitGraph);
}

void Test::TransformPos(Nav::Vector3& pos)
{
	//D3DXVECTOR3 v(pos.x, pos.y, pos.z);
	//D3DXVec3TransformCoord(&v, &v, &mWorldMtrix);
	//pos.Set(v.x, v.y, v.z);
}