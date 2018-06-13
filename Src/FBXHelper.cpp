#include "FBXHelper.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace FBXHelper
{
	static FbxManager* pFBXSDKManager = NULL;
	static FbxScene* pFBXScene = NULL;
	static FBXMeshDatas* pMeshDatas = NULL;
	static D3DXVECTOR3 boxMax;
	static D3DXVECTOR3 boxMin;

	FBXMeshDatas::~FBXMeshDatas()
	{
		for (size_t i = 0; i < datas.size(); ++i)
		{
			FBXMeshData* data = datas[i];
			SAFE_DELETE(data);
		}
		datas.clear();
	}

	void ProcessNode(FbxNode* pNode, FbxNode* pParent = NULL, int mask = -1);
	void UpdateBox(const D3DXVECTOR3& v);
	void InitBox();

	bool BeginFBXHelper(const char* fileName)
	{
		InitBox();

		InitializeSdkObjects(pFBXSDKManager, pFBXScene);

		bool rst = LoadScene(pFBXSDKManager, pFBXScene, fileName);

		if (rst == false)
		{
			FBXSDK_printf("\n\nAn error occurred while loading the scene...");
			return false;
		}

		FbxAxisSystem::DirectX.ConvertScene(pFBXScene);
		FbxAxisSystem fbxAxisSystem = pFBXScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem::ECoordSystem coordSystem = fbxAxisSystem.GetCoorSystem();
		if (coordSystem == FbxAxisSystem::ECoordSystem::eLeftHanded)
		{
			printf("Left-Hand!\n");
		}
		else
		{
			printf("Right-Hand!\n");
		}

		FbxGeometryConverter converter(pFBXSDKManager);
		converter.Triangulate(pFBXScene, true);

		pMeshDatas = new FBXMeshDatas();
		ProcessNode(pFBXScene->GetRootNode(), NULL, FbxNodeAttribute::eMesh);

		return rst;
	}

	void ProcessMesh(FbxNode* pNode)
	{
		FbxMesh* pMesh = pNode->GetMesh(); 
		if (pMesh == NULL)
		{
			return;
		}
		FBXMeshData* data = new FBXMeshData();

		int triangleCount = pMesh->GetPolygonCount();
		int vertexCount = pMesh->GetControlPointsCount();

		float scaleFactor = (float)pFBXScene->GetGlobalSettings().GetOriginalSystemUnit().GetScaleFactor();

		const FbxVector4* controlPoints = pMesh->GetControlPoints();
		for (int i = 0; i < vertexCount; ++i)
		{
			D3DXVECTOR3 v;
			FbxVector4 currentVertex = controlPoints[i];
			v.x = -(float)currentVertex[0];
			v.y = (float)currentVertex[2];
			v.z = -(float)currentVertex[1];
			v /= scaleFactor;
			UpdateBox(v);
			data->pos.push_back(v);
		}
		for (int polygonIndex = 0; polygonIndex < triangleCount; ++polygonIndex)
		{
			for (int i = 0; i < 3; ++i)
			{
				const int controlPointIndex = pMesh->GetPolygonVertex(polygonIndex, i);
				data->indices.push_back((unsigned int)controlPointIndex);
			}
		}
		pMeshDatas->datas.push_back(data);
	}

	void ProcessNode(FbxNode* pNode, FbxNode* pParent, int mask)
	{
		FbxNodeAttribute* attributeType = pNode->GetNodeAttribute();
		if (attributeType)
		{
			switch (attributeType->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
				if (mask == -1 || mask == FbxNodeAttribute::eMesh)
					ProcessMesh(pNode);
				break;
			case FbxNodeAttribute::eSkeleton:
				break;
			default:
				break;
			}
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
		{
			ProcessNode(pNode->GetChild(i), pNode, mask);
		}
	}

	FBXMeshDatas* GetMeshDatas()
	{
		return pMeshDatas;
	}

	void InitBox()
	{
		boxMax.x = FLT_MIN;
		boxMax.y = FLT_MIN;
		boxMax.z = FLT_MIN;
		boxMin.x = FLT_MAX;
		boxMin.y = FLT_MAX;
		boxMin.z = FLT_MAX;
	}

	void UpdateBox(const D3DXVECTOR3& v)
	{
		if (boxMax.x < v.x)
			boxMax.x = v.x;
		if (boxMax.y < v.y)
			boxMax.y = v.y;
		if (boxMax.z < v.z)
			boxMax.z = v.z;

		if (boxMin.x > v.x)
			boxMin.x = v.x;
		if (boxMin.y > v.y)
			boxMin.y = v.y;
		if (boxMin.z > v.z)
			boxMin.z = v.z;
	}

	void GetBox(D3DXVECTOR3& max, D3DXVECTOR3& min)
	{
		max = boxMax;
		min = boxMin;
	}

	bool EndFBXHelper()
	{
		bool rst = true;
		DestroySdkObjects(pFBXSDKManager, rst);
		pFBXSDKManager = NULL;
		pFBXScene = NULL;

		SAFE_DELETE(pMeshDatas);

		return rst;
	}
}