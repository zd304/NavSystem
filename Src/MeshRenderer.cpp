#include "MeshRenderer.h"
#include "Test.h"
#include "NavTriangle.h"
#include "NavHeightmap.h"
#include "NavGraph.h"
#include "NavGate.h"
#include "NavMesh.h"
#include "NavSystem.h"
#include "NavSceneTree.h"
#include "NavSceneNode.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

struct MeshVertex
{
	D3DXVECTOR3 pos;
	DWORD color;

	static const int fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

MeshRenderer::MeshRenderer(IDirect3DDevice9* device, Test* test)
{
	mTest = test;
	mDevice = device;
	mShowNavMesh = true;
	mShowWireMesh = true;
	mShowHeightmap = false;

	mStartMesh = NULL;
	mEndMesh = NULL;
	mSelectedMesh = NULL;

	mGateMeshInGraph = NULL;
	mGateMeshSingle = NULL;
	mAllCloseGates = NULL;

	mGraphEditMesh = NULL;
}

MeshRenderer::~MeshRenderer()
{
	mDevice = NULL;
	for (size_t i = 0; i < mMeshes.size(); ++i)
	{
		ID3DXMesh* mesh = mMeshes[i];
		SAFE_RELEASE(mesh);
	}
	mMeshes.clear();

	for (size_t i = 0; i < mWireMeshes.size(); ++i)
	{
		ID3DXMesh* mesh = mWireMeshes[i];
		SAFE_RELEASE(mesh);
	}
	mWireMeshes.clear();

	for (size_t i = 0; i < mHeightMeshes.size(); ++i)
	{
		ID3DXMesh* mesh = mHeightMeshes[i];
		SAFE_RELEASE(mesh);
	}
	mHeightMeshes.clear();

	SAFE_RELEASE(mStartMesh);
	SAFE_RELEASE(mEndMesh);
	SAFE_RELEASE(mSelectedMesh);
	mSelectedPath.clear();
	SAFE_RELEASE(mGateMeshInGraph);
	SAFE_RELEASE(mGateMeshSingle);
	SAFE_RELEASE(mGraphEditMesh);
	mSketchSceneArea.clear();
}

void MeshRenderer::Render()
{
	if (mShowNavMesh)
	{
		for (size_t i = 0; i < mMeshes.size(); ++i)
		{
			ID3DXMesh* mesh = mMeshes[i];
			if (!mesh)
			{
				continue;
			}
			mesh->DrawSubset(0);
		}
	}
	
	if (mShowHeightmap)
	{
		for (size_t i = 0; i < mHeightMeshes.size(); ++i)
		{
			ID3DXMesh* mesh = mHeightMeshes[i];
			if (!mesh)
			{
				continue;
			}
			mesh->DrawSubset(0);
		}
	}

	if (mShowWireMesh)
	{
		mDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		for (size_t i = 0; i < mWireMeshes.size(); ++i)
		{
			ID3DXMesh* mesh = mWireMeshes[i];
			if (!mesh)
			{
				continue;
			}
			mesh->DrawSubset(0);
		}
		mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	if (mAllCloseGates)
		mAllCloseGates->DrawSubset(0);

	if (mSelectedMesh)
		mSelectedMesh->DrawSubset(0);

	if (mStartMesh)
		mStartMesh->DrawSubset(0);
	if (mEndMesh)
		mEndMesh->DrawSubset(0);

	if (mSelectedPath.size() > 0)
	{
		mDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		mDevice->SetFVF(MeshVertex::fvf);
		mDevice->DrawPrimitiveUP(D3DPT_LINELIST, mSelectedPath.size() / 2, (void*)&mSelectedPath[0], sizeof(MeshVertex));
		mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	if (mGateMeshInGraph)
		mGateMeshInGraph->DrawSubset(0);
	if (mGateMeshSingle)
		mGateMeshSingle->DrawSubset(0);

	if (mGraphEditMesh)
		mGraphEditMesh->DrawSubset(0);

	if (mSketchSceneArea.size() > 0)
	{
		mDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		mDevice->SetFVF(MeshVertex::fvf);
		mDevice->DrawPrimitiveUP(D3DPT_LINELIST, mSketchSceneArea.size() / 2, &mSketchSceneArea[0], sizeof(MeshVertex));
		mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void MeshRenderer::SetMeshData(FBXHelper::FBXMeshDatas* mDatas)
{
	if (mDatas == NULL)
		return;
	for (size_t i = 0; i < mDatas->datas.size(); ++i)
	{
		FBXHelper::FBXMeshData* data = mDatas->datas[i];

		DWORD faceNum = (DWORD)(data->indices.size() / 3);
		DWORD vertexNum = (DWORD)data->pos.size();
		{
			ID3DXMesh* mesh = NULL;
			HRESULT hr = D3DXCreateMeshFVF(faceNum, vertexNum, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mesh);
			if (FAILED(hr))
				continue;
			mMeshes.push_back(mesh);
			mHeightMeshes.push_back(NULL);

			MeshVertex* vb = NULL;
			mesh->LockVertexBuffer(NULL, (void**)&vb);
			for (size_t j = 0; j < data->pos.size(); ++j)
			{
				vb[j].pos = data->pos[j];
				vb[j].color = 0xffffffff;
			}
			//memcpy(vb, &data->pos[0], vertexNum * sizeof(D3DXVECTOR3));
			mesh->UnlockVertexBuffer();

			void* ib = NULL;
			mesh->LockIndexBuffer(NULL, &ib);
			memcpy(ib, &data->indices[0], faceNum * 3 * sizeof(unsigned int));
			mesh->UnlockIndexBuffer();
		}
		{
			ID3DXMesh* mesh = NULL;
			HRESULT hr = D3DXCreateMeshFVF(faceNum, vertexNum, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mesh);
			if (FAILED(hr))
				continue;
			mWireMeshes.push_back(mesh);

			MeshVertex* vb = NULL;
			mesh->LockVertexBuffer(NULL, (void**)&vb);
			for (size_t j = 0; j < data->pos.size(); ++j)
			{
				vb[j].pos = data->pos[j];
				vb[j].color = 0xff000000;
			}
			//memcpy(vb, &data->pos[0], vertexNum * sizeof(D3DXVECTOR3));
			mesh->UnlockVertexBuffer();

			void* ib = NULL;
			mesh->LockIndexBuffer(NULL, &ib);
			memcpy(ib, &data->indices[0], faceNum * 3 * sizeof(unsigned int));
			mesh->UnlockIndexBuffer();
		}
	}
}

void MeshRenderer::SetPointMesh(const Nav::Vector3& pt0, const Nav::Vector3& pt1, const Nav::Vector3& pt2, const Nav::Vector3& point, bool isStart)
{
	ID3DXMesh** ppMesh = NULL;
	DWORD color = 0;
	if (isStart)
	{
		ppMesh = &mStartMesh;
		color = 0xffff00ff;
	}
	else
	{
		ppMesh = &mEndMesh;
		color = 0xff0000ff;
	}

	HRESULT hr = D3DXCreateMeshFVF(1, 3, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, ppMesh);
	if (FAILED(hr))
		return;
	MeshVertex* vb = NULL;
	(*ppMesh)->LockVertexBuffer(NULL, (void**)&vb);
	{
		MeshVertex& v0 = vb[0];
		v0.color = color;
		v0.pos = *(D3DXVECTOR3*)(&pt0);
		MeshVertex& v1 = vb[1];
		v1.color = color;
		v1.pos = *(D3DXVECTOR3*)(&pt1);
		MeshVertex& v2 = vb[2];
		v2.color = color;
		v2.pos = *(D3DXVECTOR3*)(&pt2);
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

void MeshRenderer::SetSelectedPath(const std::vector<Nav::NavTriangle*>& tris)
{
	SAFE_RELEASE(mSelectedMesh);

	size_t count = tris.size() * 3;
	HRESULT hr = D3DXCreateMeshFVF(tris.size(), count, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mSelectedMesh);
	if (FAILED(hr))
		return;

	MeshVertex* vb = NULL;
	mSelectedMesh->LockVertexBuffer(NULL, (void**)&vb);
	for (size_t i = 0; i < tris.size(); ++i)
	{
		Nav::NavTriangle* t = tris[i];
		MeshVertex& v0 = vb[i * 3];
		v0.color = 0xffff0000;
		v0.pos = *(D3DXVECTOR3*)(&t->mPoint[0]);
		MeshVertex& v1 = vb[i * 3 + 1];
		v1.color = 0xffff0000;
		v1.pos = *(D3DXVECTOR3*)(&t->mPoint[1]);
		MeshVertex& v2 = vb[i * 3 + 2];
		v2.color = 0xffff0000;
		v2.pos = *(D3DXVECTOR3*)(&t->mPoint[2]);
	}
	mSelectedMesh->UnlockVertexBuffer();

	unsigned int* ib = NULL;
	mSelectedMesh->LockIndexBuffer(NULL, (void**)&ib);
	for (size_t i = 0; i < tris.size(); ++i)
	{
		ib[i * 3] = i * 3;
		ib[i * 3 + 1] = i * 3 + 1;
		ib[i * 3 + 2] = i * 3 + 2;
	}
	mSelectedMesh->UnlockIndexBuffer();
}

void MeshRenderer::SetSelectedPath(const std::vector<Nav::Vector3>& path)
{
	mSelectedPath.clear();

	for (size_t i = 0; i < path.size() - 1; ++i)
	{
		Nav::Vector3 v0 = path[i];
		Nav::Vector3 v1 = path[i + 1];

		MeshVertex mv0;
		mv0.pos = *(D3DXVECTOR3*)(&v0);
		mv0.color = 0xff000000;
		MeshVertex mv1;
		mv1.pos = *(D3DXVECTOR3*)(&v1);
		mv1.color = 0xff000000;

		mSelectedPath.push_back(mv0);
		mSelectedPath.push_back(mv1);
	}
}

void MeshRenderer::SetHeightmap(const Nav::NavHeightmap* heightmap, size_t index)
{
	int verticesCount = (heightmap->mSizeX + 1) * (heightmap->mSizeZ + 1);
	int faceCount = heightmap->mSizeX * heightmap->mSizeZ * 2;
	ID3DXMesh* mesh = NULL;
	HRESULT hr = D3DXCreateMeshFVF(faceCount, verticesCount, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mesh);
	if (FAILED(hr))
		return;
	MeshVertex* vb = NULL;
	mesh->LockVertexBuffer(NULL, (void**)&vb);
	for (int i = 0; i < verticesCount; ++i)
	{
		MeshVertex& v = vb[i];
		int ix = i % (heightmap->mSizeX + 1);
		int iz = i / (heightmap->mSizeX + 1);
		v.pos.x = heightmap->mMin.x + heightmap->mCellSize.x * (float)ix;
		v.pos.z = heightmap->mMin.y + heightmap->mCellSize.y * (float)iz;
		v.pos.y = heightmap->mHeights[i];

		v.color = 0xff00ffff;
	}
	mesh->UnlockVertexBuffer();

	unsigned int* ib = NULL;
	mesh->LockIndexBuffer(NULL, (void**)&ib);
	memset(ib, 0, faceCount * 3 * sizeof(unsigned int));
	int cellCount = faceCount / 2;
	int indexIB = -1;
	for (int i = 0; i < cellCount; ++i)
	{
		if (heightmap->mCellPassability[i] == 0)
			continue;
		int col = i % heightmap->mSizeX;
		int row = i / heightmap->mSizeX;

		unsigned int index0 = row * (heightmap->mSizeX + 1) + col;
		unsigned int index1 = row * (heightmap->mSizeX + 1) + col + 1;
		unsigned int index2 = (row + 1) * (heightmap->mSizeX + 1) + col;
		unsigned int index3 = (row + 1) * (heightmap->mSizeX + 1) + col + 1;

		ib[++indexIB] = index0; ib[++indexIB] = index1; ib[++indexIB] = index2;
		ib[++indexIB] = index3; ib[++indexIB] = index2; ib[++indexIB] = index1;
	}
	mesh->UnlockIndexBuffer();
	mHeightMeshes.push_back(mesh);
}

void MeshRenderer::ClearPath()
{
	SAFE_RELEASE(mStartMesh);
	SAFE_RELEASE(mEndMesh);
	SAFE_RELEASE(mSelectedMesh);
	mSelectedPath.clear();
}

void MeshRenderer::SetGates(Nav::NavGraph* graph)
{
	SAFE_RELEASE(mGateMeshInGraph);
	SAFE_RELEASE(mGateMeshSingle);
	SAFE_RELEASE(mAllCloseGates);

	unsigned int vbCount = 0;
	for (size_t i = 0; i < graph->mGates.size(); ++i)
	{
		Nav::NavGate* gate = graph->mGates[i];
		vbCount += (gate->mTriIndices.size() * 3);
	}
	unsigned int faceCount = vbCount / 3;
	HRESULT hr = D3DXCreateMeshFVF(faceCount, vbCount, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mGateMeshInGraph);
	if (FAILED(hr))
		return;
	MeshVertex* vb = NULL;
	unsigned int* ib = NULL;
	mGateMeshInGraph->LockVertexBuffer(NULL, (void**)&vb);
	mGateMeshInGraph->LockIndexBuffer(NULL, (void**)&ib);
	int vbIndex = -1;
	for (size_t i = 0; i < graph->mGates.size(); ++i)
	{
		Nav::NavGate* gate = graph->mGates[i];
		DWORD color = gate->mPassable ? 0xffffff00 : 0x88888800;

		for (size_t j = 0; j < gate->mTriIndices.size(); ++j)
		{
			unsigned int triIndex = gate->mTriIndices[j];
			Nav::NavTriangle* tri = graph->mMesh->mTriangles[triIndex];

			MeshVertex& v0 = vb[++vbIndex];
			ib[vbIndex] = vbIndex;
			v0.pos = *(D3DXVECTOR3*)&tri->mPoint[0];
			v0.color = color;
			MeshVertex& v1 = vb[++vbIndex];
			ib[vbIndex] = vbIndex;
			v1.pos = *(D3DXVECTOR3*)&tri->mPoint[1];
			v1.color = color;
			MeshVertex& v2 = vb[++vbIndex];
			ib[vbIndex] = vbIndex;
			v2.pos = *(D3DXVECTOR3*)&tri->mPoint[2];
			v2.color = color;
		}
	}
	mGateMeshInGraph->UnlockVertexBuffer();
	mGateMeshInGraph->UnlockIndexBuffer();
}

void MeshRenderer::SetSingleGate(Nav::NavGraph* graph, Nav::NavGate* gate)
{
	SAFE_RELEASE(mGateMeshInGraph);
	SAFE_RELEASE(mGateMeshSingle);
	SAFE_RELEASE(mAllCloseGates);

	unsigned int vbCount = gate->mTriIndices.size() * 3;
	unsigned int faceCount = vbCount / 3;
	HRESULT hr = D3DXCreateMeshFVF(faceCount, vbCount, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mSelectedMesh);
	if (FAILED(hr))
		return;
	MeshVertex* vb = NULL;
	unsigned int* ib = NULL;

	mSelectedMesh->LockVertexBuffer(NULL, (void**)&vb);
	mSelectedMesh->LockIndexBuffer(NULL, (void**)&ib);
	int vbIndex = -1;

	DWORD color = gate->mPassable ? 0xffffff00 : 0x88888800;
	for (size_t j = 0; j < gate->mTriIndices.size(); ++j)
	{
		unsigned int triIndex = gate->mTriIndices[j];
		Nav::NavTriangle* tri = graph->mMesh->mTriangles[triIndex];

		MeshVertex& v0 = vb[++vbIndex];
		ib[vbIndex] = vbIndex;
		v0.pos = *(D3DXVECTOR3*)&tri->mPoint[0];
		v0.color = color;
		MeshVertex& v1 = vb[++vbIndex];
		ib[vbIndex] = vbIndex;
		v1.pos = *(D3DXVECTOR3*)&tri->mPoint[1];
		v1.color = color;
		MeshVertex& v2 = vb[++vbIndex];
		ib[vbIndex] = vbIndex;
		v2.pos = *(D3DXVECTOR3*)&tri->mPoint[2];
		v2.color = color;
	}

	mSelectedMesh->UnlockVertexBuffer();
	mSelectedMesh->UnlockIndexBuffer();
}

void MeshRenderer::ClearGate()
{
	SAFE_RELEASE(mGateMeshInGraph);
	SAFE_RELEASE(mGateMeshSingle);
	CalcAllCloseGates();
}

void MeshRenderer::CalcAllCloseGates()
{
	SAFE_RELEASE(mAllCloseGates);

	DWORD color = 0x88888800;
	unsigned int vbCount = 0;
	for (size_t i = 0; i < mTest->mNavSystem->GetGraphCount(); ++i)
	{
		Nav::NavGraph* graph = mTest->mNavSystem->GetGraphByIndex(i);
		for (size_t j = 0; j < graph->mGates.size(); ++j)
		{
			Nav::NavGate* gate = graph->mGates[j];
			if (gate->mPassable)
				continue;
			vbCount += gate->mTriIndices.size() * 3;
		}
	}
	unsigned int faceCount = vbCount / 3;

	HRESULT hr = D3DXCreateMeshFVF(faceCount, vbCount, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mAllCloseGates);
	if (FAILED(hr))
		return;
	MeshVertex* vb = NULL;
	unsigned int* ib = NULL;

	mAllCloseGates->LockVertexBuffer(NULL, (void**)&vb);
	mAllCloseGates->LockIndexBuffer(NULL, (void**)&ib);
	int vbIndex = -1;
	for (size_t i = 0; i < mTest->mNavSystem->GetGraphCount(); ++i)
	{
		Nav::NavGraph* graph = mTest->mNavSystem->GetGraphByIndex(i);
		for (size_t j = 0; j < graph->mGates.size(); ++j)
		{
			Nav::NavGate* gate = graph->mGates[j];
			if (gate->mPassable)
				continue;
			for (size_t k = 0; k < gate->mTriIndices.size(); ++k)
			{
				unsigned int triIndex = gate->mTriIndices[k];
				Nav::NavTriangle* tri = graph->mMesh->mTriangles[triIndex];

				MeshVertex& v0 = vb[++vbIndex];
				ib[vbIndex] = vbIndex;
				v0.pos = *(D3DXVECTOR3*)&tri->mPoint[0];
				v0.color = color;
				MeshVertex& v1 = vb[++vbIndex];
				ib[vbIndex] = vbIndex;
				v1.pos = *(D3DXVECTOR3*)&tri->mPoint[1];
				v1.color = color;
				MeshVertex& v2 = vb[++vbIndex];
				ib[vbIndex] = vbIndex;
				v2.pos = *(D3DXVECTOR3*)&tri->mPoint[2];
				v2.color = color;
			}
		}
	}
	mAllCloseGates->UnlockVertexBuffer();
	mAllCloseGates->UnlockIndexBuffer();
}

void MeshRenderer::SetEditGraph(const Nav::NavGraph* graph)
{
	SAFE_RELEASE(mGraphEditMesh);

	if (graph == NULL)
		return;

	unsigned int vbCount = graph->mMesh->mTriangles.size();
	HRESULT hr = D3DXCreateMeshFVF(vbCount / 3, vbCount, D3DXMESH_32BIT, MeshVertex::fvf, mDevice, &mGraphEditMesh);
	if (FAILED(hr))
		return;

	DWORD color = 0x88000000;

	MeshVertex* vb = NULL;
	unsigned int* ib = NULL;

	mGraphEditMesh->LockVertexBuffer(NULL, (void**)&vb);
	mGraphEditMesh->LockIndexBuffer(NULL, (void**)&ib);
	int vbIndex = -1;
	for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
	{
		Nav::NavTriangle* tri = graph->mMesh->mTriangles[i];
		for (int j = 0; j < 3; ++j)
		{
			MeshVertex& v = vb[++vbIndex];
			v.pos = *(D3DXVECTOR3*)&tri->mPoint[j];
			v.color = color;
			ib[vbIndex] = vbIndex;
		}
	}

	mGraphEditMesh->UnlockIndexBuffer();
	mGraphEditMesh->UnlockVertexBuffer();
}

void MeshRenderer::SetSketchScene(Nav::NavSceneTree* tree)
{
	mSketchSceneArea.clear();
	if (!tree)
		return;

	std::map<unsigned int, Nav::NavSceneNode*>::iterator it;
	for (it = tree->mNodes.begin(); it != tree->mNodes.end(); ++it)
	{
		Nav::NavSceneNode* node = it->second;
		MeshVertex v[4];

		v[0].pos = D3DXVECTOR3(node->x, 0.0f, node->y);
		v[0].color = 0xff0000ff;

		v[1].pos = D3DXVECTOR3(node->x, 0.0f, node->y + node->height);
		v[1].color = 0xff0000ff;

		v[3].pos = D3DXVECTOR3(node->x + node->width, 0.0f, node->y);
		v[3].color = 0xff0000ff;

		v[2].pos = D3DXVECTOR3(node->x + node->width, 0.0f, node->y + node->height);
		v[2].color = 0xff0000ff;

		mSketchSceneArea.push_back(v[0]);
		mSketchSceneArea.push_back(v[1]);

		mSketchSceneArea.push_back(v[1]);
		mSketchSceneArea.push_back(v[2]);

		mSketchSceneArea.push_back(v[2]);
		mSketchSceneArea.push_back(v[3]);

		mSketchSceneArea.push_back(v[3]);
		mSketchSceneArea.push_back(v[0]);
	}
}