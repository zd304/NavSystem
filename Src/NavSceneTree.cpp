#include "NavSceneTree.h"
#include "NavSceneNode.h"
#include "NavGraph.h"
#include "NavMesh.h"
#include "NavLinkInfo.h"
#include "NavSystem.h"
#include "NavTriangle.h"

namespace Nav
{
	static const float MAX_COST = 1e4f;

	NavSceneTree::NavSceneTree(NavSystem* navSys)
	{
		mNavSystem = navSys;
		mQuadTree = NULL;
		mPather = new micropather::MicroPather(this);
	}

	NavSceneTree::~NavSceneTree()
	{
		SAFE_DELETE(mPather);
		Clear();
	}

	void NavSceneTree::Clear()
	{
		std::map<unsigned int, NavSceneNode*>::iterator it;
		for (it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			NavSceneNode* node = it->second;
			SAFE_DELETE(node);
		}
		mNodes.clear();
		SAFE_DELETE(mQuadTree);
	}

	float NavSceneTree::LeastCostEstimate(void* stateStart, void* stateEnd)
	{
		if (!stateStart || !stateEnd)
			return MAX_COST;

		NavSceneNode* graphStart = (NavSceneNode*)stateStart;
		NavSceneNode* graphEnd = (NavSceneNode*)stateEnd;

		float cost = (graphStart->GetCenter() - graphEnd->GetCenter()).Length();

		return cost;
	}

	void NavSceneTree::AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent)
	{
		if (!state) return;
		NavSceneNode* scnNode = (NavSceneNode*)state;
		for (size_t i = 0; i < scnNode->mLinkScnIDs.size(); ++i)
		{
			unsigned int scnID = scnNode->mLinkScnIDs[i];

			std::map<unsigned int, NavSceneNode*>::iterator it;
			it = mNodes.find(scnID);
			if (it == mNodes.end())
				continue;

			NavSceneNode* adjacentScnNode = it->second;

			micropather::StateCost cost;
			cost.state = (void*)adjacentScnNode;
			cost.cost = (scnNode->GetCenter() - adjacentScnNode->GetCenter()).Length();

			adjacent->push_back(cost);
		}
	}

	void NavSceneTree::PrintStateInfo(void* state)
	{
		// Empty;
	}

	bool NavSceneTree::AddSceneNode(NavSceneNode* node)
	{
		std::map<unsigned int, NavSceneNode*>::iterator it;
		it = mNodes.find(node->mScnID);
		if (it != mNodes.end())
		{
			return false;
		}
		mNodes[node->mScnID] = node;
		mQuadTree->InsertObject(node);
		return true;
	}

	void NavSceneTree::InitSceneTree(float x, float y, float width, float height, int maxLevel)
	{
		Clear();

		mQuadTree = new QuadTreeNode<NavSceneNode>(x, y, width, height, 1, maxLevel, ROOT, NULL);
	}

	bool NavSceneTree::Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost, bool smoothPath) const
	{
		NavSceneNode* startScnNode = GetScnNodeByPoint(start);
		NavSceneNode* endScnNode = GetScnNodeByPoint(end);

		if (startScnNode == endScnNode)
		{
			NavGraph* startGraph = GetGraphByPoint(start, startScnNode);
			if (!startGraph)
				return false;

			return startGraph->Solve(start, end, path, cost, smoothPath);
		}

		std::vector<NavSceneNode*> scnNodes;
		int rst = mPather->Solve((void*)startScnNode, (void*)endScnNode, (std::vector<void*>*)&scnNodes, cost);
		if (MAX_COST <= *cost)
			return false;

		bool rstBool = false;
		if (rst == micropather::MicroPather::SOLVED
			|| rst == micropather::MicroPather::START_END_SAME)
			rstBool = true;

		NavGraph* startGraph = GetGraphByPoint(start, startScnNode);
		Vector3 targetPos = end;

		if (rst == micropather::MicroPather::SOLVED)
		{
			NavSceneNode* nextScnNode = scnNodes[1];
			unsigned int nextSceneID = nextScnNode->mScnID;
			float targetMinDist = FLT_MAX;
			
			for (size_t i = 0; i < startGraph->mMesh->mNavLinkInfos.size(); ++i)
			{
				NavLinkInfo* linkInfo = startGraph->mMesh->mNavLinkInfos[i];
				if (linkInfo->mLinkID == nextSceneID)
				{
					unsigned int targetTriIndex = linkInfo->mTriIndex;
					NavTriangle* tri = startGraph->mMesh->mTriangles[targetTriIndex];
					Vector3 center = (tri->mPoint[0] + tri->mPoint[1] + tri->mPoint[2]) / 3.0f;

					float dist = (center - start).Length();
					if (dist < targetMinDist)
					{
						targetMinDist = dist;
						targetPos = center;
					}
				}
			}

			if (targetMinDist == FLT_MAX)
				return false;
		}
		if (rstBool)
		{
			return startGraph->Solve(start, targetPos, path, cost, smoothPath);
		}
		return rstBool;
	}

	NavSceneNode* NavSceneTree::GetScnNodeByPoint(const Vector3& p) const
	{
		std::list<NavSceneNode*> graphs = mQuadTree->GetObjectsAt(p.x, p.z);

		if (graphs.size() > 0)
		{
			return *graphs.begin();
		}
		return NULL;
	}

	NavGraph* NavSceneTree::GetGraphByPoint(const Vector3& p, const NavSceneNode* scnNode) const
	{
		NavGraph* rst = NULL;
		float maxHeight = FLT_MIN;
		for (unsigned int i = 0; i < mNavSystem->GetGraphCount(); ++i)
		{
			NavGraph* graph = mNavSystem->GetGraphByIndex(i);
			if (!graph || !graph->mMesh)
				continue;
			if (graph->GetSceneID() != scnNode->mScnID)
				continue;
			NavTriangle* tri = graph->GetTriangleByPoint(p);
			if (!tri) continue;

			float h = (tri->mPoint[0].y + tri->mPoint[1].y + tri->mPoint[2].y) / 3.0f;
			if (h > p.y + 0.5f) continue;

			if (h > maxHeight)
			{
				maxHeight = h;
				rst = graph;
			}
		}
		return rst;
	}

	unsigned int NavSceneTree::GetSize()
	{
		unsigned int size = sizeof(float) * 4;
		size += sizeof(int);

		size += sizeof(unsigned int);
		std::map<unsigned int, NavSceneNode*>::iterator it;
		for (it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			NavSceneNode* scnNode = it->second;
			size += scnNode->GetSize();
		}
		return size;
	}

	unsigned int NavSceneTree::WriteTo(char* dest, unsigned int ptr)
	{
		float x, y, width, height;
		mQuadTree->GetRect(&x, &y, &width, &height);
		int maxLevel;
		mQuadTree->GetMaxLevel(&maxLevel);

		memcpy(dest + ptr, &x, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &y, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &width, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &height, sizeof(float));
		ptr += sizeof(float);
		memcpy(dest + ptr, &maxLevel, sizeof(int));
		ptr += sizeof(int);

		unsigned int scnCount = (unsigned int)mNodes.size();
		memcpy(dest + ptr, &scnCount, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		std::map<unsigned int, NavSceneNode*>::iterator it;
		for (it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			NavSceneNode* scnNode = it->second;
			ptr = scnNode->WriteTo(dest, ptr);
		}
		return ptr;
	}

	unsigned int NavSceneTree::ReadFrom(char* src, unsigned int ptr)
	{
		float x, y, width, height;
		int maxLevel;

		memcpy(&x, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&y, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&width, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&height, src + ptr, sizeof(float));
		ptr += sizeof(float);
		memcpy(&maxLevel, src + ptr, sizeof(int));
		ptr += sizeof(int);

		InitSceneTree(x, y, width, height, maxLevel);

		unsigned int scnCount = 0;
		memcpy(&scnCount, src + ptr, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		for (unsigned int i = 0; i < scnCount; ++i)
		{
			NavSceneNode* node = new NavSceneNode();

			ptr = node->ReadFrom(src, ptr);

			AddSceneNode(node);
		}

		return ptr;
	}
}