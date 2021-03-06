#ifndef __NAV_SCENE_H__
#define __NAV_SCENE_H__

#include "NavInc.h"

namespace Nav
{
	class NavSystem;
	class NavSceneNode;
	class NavGraph;

	class NavSceneTree : public micropather::Graph
	{
	public:
		NavSceneTree(NavSystem* navSys);

		~NavSceneTree();

		void Clear();

		bool AddSceneNode(NavSceneNode* node);

		void InitSceneTree(float x, float y, float width, float height, int maxLevel);

		bool Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost, bool smoothPath) const;

		unsigned int GetSize();

		unsigned int WriteTo(char* dest, unsigned int ptr);

		unsigned int ReadFrom(char* src, unsigned int ptr);
	public:
		// Inherit;
		float LeastCostEstimate(void* stateStart, void* stateEnd);
		void AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent);
		void PrintStateInfo(void* state);
	private:
		NavSceneNode* GetScnNodeByPoint(const Vector3& p) const;

		NavGraph* GetGraphBySceneNode(const NavSceneNode* scnNode) const;

		bool GetGraphLinkPosition(const NavGraph* graph, const Vector3& startPos, unsigned int linkID, Vector3& rst) const;
	private:
		NavSystem* mNavSystem;
		micropather::MicroPather* mPather;
	public:
		std::map<unsigned int, NavSceneNode*> mNodes;
		QuadTreeNode<NavSceneNode>* mQuadTree;
	};
}

#endif
