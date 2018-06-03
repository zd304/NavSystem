#include "MeshRenderer.h"

struct MeshVertex
{
	D3DXVECTOR3 pos;
	DWORD color;

	static const int fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

MeshRenderer::MeshRenderer(IDirect3DDevice9* device, FBXHelper::FBXMeshDatas* mDatas)
{
	mDevice = device;
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
				vb[j].pos.y += 0.1f;
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
}

void MeshRenderer::Render()
{
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
	mDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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