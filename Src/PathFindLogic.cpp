#include "PathFindLogic.h"
#include "Test.h"
#include "NavTriangle.h"
#include "NavGraph.h"
#include "MeshRenderer.h"

PathFindLogic::PathFindLogic(Test* test)
{
	mTest = test;
	mShowTriPath = true;
	mSmoothPath = true;
}

PathFindLogic::~PathFindLogic()
{
	if (mTest->mRenderer)
		mTest->mRenderer->ClearPath();
}

void PathFindLogic::OnPick(const NavTriangle* tri, const Vector3& point, const NavGraph* graph)
{
	if (mTest->mRenderer == NULL)
		return;
	if (tri == NULL)
	{
		mClickMode = eClickState::eClickState_None;
		ClearPath();
		return;
	}
	if (mStartTri == NULL && mEndTri == NULL)
	{
		mClickMode = eClickState::eClickState_Start;
	}
	if (mStartTri != NULL && mEndTri == NULL)
	{
		mClickMode = eClickState::eClickState_End;
	}
	if (mStartTri != NULL && mEndTri != NULL)
	{
		mClickMode = eClickState::eClickState_Start;
		ClearPath();
	}

	if (mClickMode == eClickState_Start)
	{
		SetPointMesh(tri, point, true);
		return;
	}
	else if (mClickMode == eClickState_End)
	{
		SetPointMesh(tri, point, false);

		const NavGraph* finder = graph;
		if (mStartTri != NULL && mEndTri != NULL)
		{
			std::vector<NavTriangle*> findPath;
			float cost;
			if (mShowTriPath)
			{
				if (finder->Solve(mStartTri, mEndTri, &findPath, &cost))
				{
					mTest->mRenderer->SetSelectedPath(findPath);
				}
			}
			std::vector<Vector3> findVectorPath;
			if (finder->Solve(mStartPoint, mEndPoint, &findVectorPath, &cost, mSmoothPath))
			{
				mTest->mRenderer->SetSelectedPath(findVectorPath);
			}
		}

		return;
	}
}

void PathFindLogic::OnGUI()
{
	ImGui::Spacing();
	ImGui::Text(STU("第一次点击模型，确定寻路起点\n第二次点击导航，确定寻路终点\n自动生成路径").c_str());
	ImGui::Spacing();

	std::string text = mSmoothPath ? "关闭路径平滑" : "开启路径平滑";
	if (ImGui::Button(STU(text).c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 30.0f)))
	{
		mSmoothPath = !mSmoothPath;
	}
}

void PathFindLogic::SetPointMesh(const NavTriangle* tri, const Vector3& point, bool isStart)
{
	if (mTest->mRenderer == NULL)
		return;
	ID3DXMesh** ppMesh = NULL;
	DWORD color = 0;
	if (isStart)
	{
		mStartTri = (NavTriangle*)tri;
		mStartPoint = point;
	}
	else
	{
		mEndTri = (NavTriangle*)tri;
		mEndPoint = point;
	}
	mTest->mRenderer->SetPointMesh(tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], point, isStart);
}

void PathFindLogic::ClearPath()
{
	mStartTri = NULL;
	mEndTri = NULL;
	mClickMode = eClickState_None;

	if (mTest->mRenderer)
		mTest->mRenderer->ClearPath();
}