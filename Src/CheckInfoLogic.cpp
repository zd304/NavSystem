#include "CheckInfoLogic.h"
#include "Nav.h"
#include "Test.h"
#include "MeshRenderer.h"

CheckInfoLogic::CheckInfoLogic(Test* test)
{
	mTest = test;
	mGraph = NULL;
	mTri = NULL;
}

CheckInfoLogic::~CheckInfoLogic()
{
	if (!mTest->mRenderer)
		return;
	mTest->mRenderer->ClearPath();
}

void CheckInfoLogic::OnPick(const Nav::NavTriangle* tri, const Nav::Vector3& point, const Nav::NavGraph* graph)
{
	if (!mTest->mRenderer)
		return;
	if (mTri)
	{
		mTri = NULL;
		mTest->mRenderer->ClearPath();
		return;
	}
	if (tri != NULL && graph != NULL)
	{
		mTri = (Nav::NavTriangle*)tri;
		mGraph = (Nav::NavGraph*)graph;
		mTest->mRenderer->SetPointMesh(tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], point, true);
	}
}

void CheckInfoLogic::OnGUI()
{
	if (!mTri || !mGraph) return;

	unsigned int triIndex = -1;
	for (size_t i = 0; i < mGraph->mMesh->mTriangles.size(); ++i)
	{
		if (mGraph->mMesh->mTriangles[i] == mTri)
		{
			triIndex = (unsigned int)i;
			break;
		}
	}

	float distance[3];
	memset(distance, 0, sizeof(float) * 3);
	for (size_t i = 0; i < mTri->mDistance.size(); ++i)
	{
		distance[i] = mTri->mDistance[i];
	}

	char szContent[1024];
	memset(szContent, 0, 1024);
	sprintf_s(szContent, "\n三角形[%d]:\n  (%.4f, %.4f, %.4f)\n  (%.4f, %.4f, %.4f)\n  (%.4f, %.4f, %.4f)\n邻接三角形数量:  %d\n到邻接三角形距离:\n  %.4f, %.4f, %.4f\n三角形是否可通行:  %s\n所处导航ID:  %d, 场景ID:  %d",
		triIndex,
		mTri->mPoint[0].x, mTri->mPoint[0].y, mTri->mPoint[0].z,
		mTri->mPoint[1].x, mTri->mPoint[1].y, mTri->mPoint[1].z,
		mTri->mPoint[2].x, mTri->mPoint[2].y, mTri->mPoint[2].z,
		(unsigned int)mTri->mNeighbors.size(),
		distance[0], distance[1], distance[2],
		mTri->mPassable ? "是" : "否",
		mGraph->GetLayerID(), mGraph->GetSceneID());
	ImGui::Text(STU(szContent).c_str());
}