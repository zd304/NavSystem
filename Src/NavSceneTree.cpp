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
			NavGraph* startGraph = GetGraphBySceneNode(startScnNode);
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

		if (rst == micropather::MicroPather::SOLVED)
		{
			rstBool = false;
			size_t scnNodeSize = scnNodes.size();

			Vector3 startPos = start;

			for (size_t s = 0; s < scnNodeSize; ++s)
			{
				NavSceneNode* scnNode = scnNodes[s];

				NavGraph* graph = GetGraphBySceneNode(scnNode);
				if (!graph) break;

				Vector3 targetPos = end;
				
				if (s + 1 < scnNodeSize)
				{
					// 查找targetPos;
					NavSceneNode* nextScnNode = scnNodes[s + 1];
					unsigned int nextSceneID = nextScnNode->mScnID;
					float targetMinDist = FLT_MAX;

					for (size_t i = 0; i < graph->mMesh->mNavLinkInfos.size(); ++i)
					{
						NavLinkInfo* linkInfo = graph->mMesh->mNavLinkInfos[i];
						if (linkInfo->mLinkID == nextSceneID)
						{
							unsigned int targetTriIndex = linkInfo->mTriIndex;
							NavTriangle* tri = graph->mMesh->mTriangles[targetTriIndex];
							Vector3 center = tri->mCenter;

							float dist = (center - start).Length();
							if (dist < targetMinDist)
							{
								targetMinDist = dist;
								targetPos = center;
							}
						}
					}
					if (targetMinDist < 0.0f)
						return false;

					std::vector<Vector3> subPath;
					if (graph->Solve(startPos, targetPos, &subPath, cost, smoothPath))
					{
						for (size_t j = 0; j < subPath.size(); ++j)
						{
							path->push_back(subPath[j]);
						}
						rstBool = true;
					}

					// 查找下一个场景的startPos;
					targetMinDist = FLT_MAX;

					NavGraph* nextGraph = GetGraphBySceneNode(nextScnNode);
					if (nextGraph)
					{
						for (size_t i = 0; i < nextGraph->mMesh->mNavLinkInfos.size(); ++i)
						{
							NavLinkInfo* linkInfo = nextGraph->mMesh->mNavLinkInfos[i];
							if (linkInfo->mLinkID == graph->mID)
							{
								unsigned int targetTriIndex = linkInfo->mTriIndex;
								NavTriangle* tri = nextGraph->mMesh->mTriangles[targetTriIndex];
								Vector3 center = tri->mCenter;

								float dist = (center - start).Length();
								if (dist < targetMinDist)
								{
									targetMinDist = dist;
									startPos = center;
								}
							}
						}
					}
				}
				else
				{
					std::vector<Vector3> subPath;
					if (graph->Solve(startPos, targetPos, &subPath, cost, smoothPath))
					{
						for (size_t j = 0; j < subPath.size(); ++j)
						{
							path->push_back(subPath[j]);
						}
						rstBool = true;
					}
				}
			}
		}
		return rstBool;
	}

	NavSceneNode* NavSceneTree::GetScnNodeByPoint(const Vector3& p) const
	{
		std::list<NavSceneNode*> graphs = mQuadTree->GetObjectsAt(p.x, p.z);

		if (graphs.size() > 0)
		{
			NavSceneNode* rst = NULL;
			float maxHeight = -9999999.9f;

			std::list<NavSceneNode*>::iterator it;
			for (it = graphs.begin(); it != graphs.end(); ++it)
			{
				NavSceneNode* node = *it;
				if (!node)
					continue;
				NavGraph* graph = mNavSystem->GetGraphByID(node->mScnID);
				if (!graph) continue;
				NavTriangle* tri = graph->GetTriangleByPoint(p);
				if (!tri) continue;
				if (maxHeight < tri->mCenter.y)
				{
					maxHeight = tri->mCenter.y;
					rst = node;
				}
			}
			return rst;
		}
		return NULL;
	}

	NavGraph* NavSceneTree::GetGraphBySceneNode(const NavSceneNode* scnNode) const
	{
		NavGraph* graph = mNavSystem->GetGraphByID(scnNode->mScnID);
		if (!graph || !graph->mMesh)
			return NULL;

		return graph;
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