#include "EdgeLogic.h"
#include "NavGraph.h"
#include "NavMesh.h"
#include "NavTriangle.h"
#include "NavEdge.h"
#include "NavPhysics.h"
#include "Test.h"
#include "MeshRenderer.h"

EdgeLogic::EdgeLogic(Test* test)
{
	mTest = test;
	mGraph = NULL;
	selBoundsIndex = -1;
}

EdgeLogic::~EdgeLogic()
{
	Clear();
}

void EdgeLogic::Clear()
{
	selBoundsIndex = -1;
	if (mTest && mTest->mRenderer)
	{
		mTest->mRenderer->ClearBounds();
	}
	for (size_t i = 0; i < mDelBounds.size(); ++i)
	{
		Nav::NavEdge* edge = mDelBounds[i];
		SAFE_DELETE(edge);
	}
	mDelBounds.clear();
}

void EdgeLogic::OnPick(const Nav::NavTriangle* tri, const Nav::Vector3& point, const Nav::NavGraph* graph)
{
	if (mGraph)
	{
		return;
	}
	if (graph == NULL)
	{
		//Clear();
		return;
	}
	if (!graph->mMesh)
		return;

	mGraph = const_cast<Nav::NavGraph*>(graph);

	mDelBounds.clear();
	for (unsigned int i = 0; i < mGraph->mMesh->mTriangles.size(); ++i)
	{
		Nav::NavTriangle* tri = mGraph->mMesh->mTriangles[i];

		bool hasEdge[3] { false, false, false };
		for (unsigned int j = 0; j < tri->mShareEdgeIndices.size(); ++j)
		{
			int edgeIndex = tri->mShareEdgeIndices[j];
			hasEdge[edgeIndex] = true;
		}
		for (int j = 0; j < 3; ++j)
		{
			if (hasEdge[j])
				continue;
			int edgeIndex = j;

			int ptIndex1 = -1;
			int ptIndex2 = -1;
			Nav::DecodeEdgeIndex(edgeIndex, &ptIndex1, &ptIndex2);

			bool find = false;
			for (unsigned int k = 0; k < mGraph->mMesh->mBounds.size(); ++k)
			{
				Nav::NavEdge* e = mGraph->mMesh->mBounds[k];
				bool del = false;
				if (e->mPoint[0] == tri->mPoint[ptIndex1] && e->mPoint[1] == tri->mPoint[ptIndex2])
				{
					find = true;
					break;
				}
				if (e->mPoint[1] == tri->mPoint[ptIndex1] && e->mPoint[0] == tri->mPoint[ptIndex2])
				{
					find = true;
					break;
				}
			}
			if (!find)
			{
				Nav::NavEdge* edge = new Nav::NavEdge();
				edge->mPoint[0] = tri->mPoint[ptIndex1];
				edge->mPoint[1] = tri->mPoint[ptIndex2];
				mDelBounds.push_back(edge);
			}
		}
	}

	mTest->mRenderer->SetBounds(mGraph->mMesh->mBounds, mDelBounds, NULL);
}

void EdgeLogic::OnPick(const Nav::Vector3& orig, const Nav::Vector3& vDir)
{
	if (!mGraph)
	{
		return;
	}

	Nav::Vector3 vb[4];

	std::vector<Nav::NavEdge*>& curBounds = mGraph->mMesh->mBounds;
	for (size_t i = 0; i < curBounds.size(); ++i)
	{
		Nav::NavEdge* e = curBounds[i];

		Nav::Vector3 dir = e->mPoint[1] - e->mPoint[0];

		if (dir == Nav::Vector3::ZERO || dir == Nav::Vector3::UP)
			dir.Set(0.0f, 0.0f, 1.0f);

		dir.Normalize();

		Nav::Vector3 r;
		Nav::Vector3::Vector3Cross(r, Nav::Vector3::UP, dir);
		r = r * 0.5f;

		vb[0] = e->mPoint[1] + 5.0f * r;
		vb[1] = e->mPoint[1] - 5.0f * r;
		vb[2] = e->mPoint[0] - 5.0f * r;
		vb[3] = e->mPoint[0] + 5.0f * r;

		NavPhysics::NavHit hitInfo;
		bool sel = NavPhysics::RayIntersectTriangle(orig, vDir, vb[2], vb[1], vb[0], &hitInfo);
		sel |= NavPhysics::RayIntersectTriangle(orig, vDir, vb[2], vb[0], vb[3], &hitInfo);
		
		if (sel == false)
			continue;

		selBoundsIndex = i;
		mTest->mRenderer->SetBounds(mGraph->mMesh->mBounds, mDelBounds, e);
		break;
	}

	for (size_t i = 0; i < mDelBounds.size(); ++i)
	{
		Nav::NavEdge* e = mDelBounds[i];

		Nav::Vector3 dir = e->mPoint[1] - e->mPoint[0];

		if (dir == Nav::Vector3::ZERO || dir == Nav::Vector3::UP)
			dir.Set(0.0f, 0.0f, 1.0f);

		dir.Normalize();

		Nav::Vector3 r;
		Nav::Vector3::Vector3Cross(r, Nav::Vector3::UP, dir);
		r = r * 0.5f;

		vb[0] = e->mPoint[1] + 5.0f * r;
		vb[1] = e->mPoint[1] - 5.0f * r;
		vb[2] = e->mPoint[0] - 5.0f * r;
		vb[3] = e->mPoint[0] + 5.0f * r;

		NavPhysics::NavHit hitInfo;
		bool sel = NavPhysics::RayIntersectTriangle(orig, vDir, vb[0], vb[1], vb[2], &hitInfo);
		sel |= NavPhysics::RayIntersectTriangle(orig, vDir, vb[3], vb[0], vb[2], &hitInfo);

		if (sel == false)
			continue;

		selBoundsIndex = i + curBounds.size();
		mTest->mRenderer->SetBounds(mGraph->mMesh->mBounds, mDelBounds, e);
		break;
	}
}

void EdgeLogic::OnGUI()
{
	if (!mGraph) return;

	ImGui::Text(STU("ÒÑ´æÔÚµÄ±ß").c_str());
	ImGui::Spacing();
	std::vector<Nav::NavEdge*>& curBounds = mGraph->mMesh->mBounds;
	for (size_t i = 0; i < curBounds.size(); ++i)
	{
		Nav::NavEdge* e = curBounds[i];

		ImGui::Text(STU("±ß[%02d]").c_str(), i);
		ImGui::SameLine(80.0f);
		ImGui::PushID(i);
		if (ImGui::Button(STU("±à¼­").c_str()))
		{
			mTest->mRenderer->SetBounds(mGraph->mMesh->mBounds, mDelBounds, e);
			selBoundsIndex = i;
		}
		ImGui::PopID();
		if (selBoundsIndex == i)
		{
			if (ImGui::Button(STU("É¾³ý").c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 25.0f)))
			{
				curBounds.erase(curBounds.begin() + i);
				mDelBounds.push_back(e);
				break;
			}
		}
	}

	ImGui::Separator();

	ImGui::Text(STU("ÒÑÉ¾³ýµÄ±ß").c_str());
	ImGui::Spacing();
	for (size_t i = 0; i < mDelBounds.size(); ++i)
	{
		Nav::NavEdge* e = mDelBounds[i];
		ImGui::Text(STU("±ß[%02d]").c_str(), i);
		ImGui::SameLine(80.0f);
		ImGui::PushID(i + curBounds.size());
		if (ImGui::Button(STU("±à¼­").c_str()))
		{
			mTest->mRenderer->SetBounds(mGraph->mMesh->mBounds, mDelBounds, e);
			selBoundsIndex = i + curBounds.size();
		}
		ImGui::PopID();
		if (selBoundsIndex == i + curBounds.size())
		{
			if (ImGui::Button(STU("Ìí¼Ó").c_str(), ImVec2(mTest->mLeftUIWidth - 20.0f, 25.0f)))
			{
				mDelBounds.erase(curBounds.begin() + i);
				curBounds.push_back(e);
				break;
			}
		}
	}
}