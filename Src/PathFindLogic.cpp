#include "PathFindLogic.h"
#include "Test.h"
#include "NavTriangle.h"
#include "NavGraph.h"
#include "MeshRenderer.h"

PathFindLogic::PathFindLogic(Test* test)
{
	mTest = test;
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
			if (finder->Solve(mStartTri, mEndTri, &findPath, &cost))
			{
				mTest->mRenderer->SetSelectedPath(findPath);
			}
			std::vector<Vector3> findVectorPath;
			if (finder->Solve(mStartPoint, mEndPoint, &findVectorPath, &cost))
			{
				mTest->mRenderer->SetSelectedPath(findVectorPath);
			}
		}

		return;
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