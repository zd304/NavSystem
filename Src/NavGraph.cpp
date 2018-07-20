#include "NavGraph.h"
#include "NavEdge.h"
#include "NavTriangle.h"
#include "NavMesh.h"
#include "NavHeightmap.h"
#include "NavGate.h"
#include "NavPhysics.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace Nav
{
	static const float MAX_COST = 1e4f;

	NavGraph::NavGraph()
	{
		mID = 0;
		mPather = new micropather::MicroPather(this);
		mHeightmap = NULL;
		mMesh = NULL;
	}

	NavGraph::NavGraph(NavMesh* mesh)
	{
		mID = 0;
		mPather = new micropather::MicroPather(this);
		mHeightmap = new NavHeightmap();
		mHeightmap->BuildHeightmap(mesh);
		mMesh = mesh;
	}

	NavGraph::~NavGraph()
	{
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			SAFE_DELETE(gate);
		}
		mGates.clear();
		SAFE_DELETE(mPather);
		SAFE_DELETE(mMesh);
		SAFE_DELETE(mHeightmap);
	}

	float NavGraph::LeastCostEstimate(void* stateStart, void* stateEnd)
	{
		if (!stateStart || !stateEnd)
			return MAX_COST;

		NavTriangle* triStart = (NavTriangle*)stateStart;
		NavTriangle* triEnd = (NavTriangle*)stateEnd;
		if (!triStart->mPassable || !triEnd->mPassable)
			return MAX_COST;

		float cost = (triStart->mCenter - triEnd->mCenter).Length();

		return cost;
	}

	void NavGraph::AdjacentCost(void* state, std::vector<micropather::StateCost> *adjacent)
	{
		if (!state) return;
		NavTriangle* tri = (NavTriangle*)state;
		for (unsigned int i = 0; i < tri->mNeighbors.size(); ++i)
		{
			NavTriangle* neighbor = tri->mNeighbors[i];

			micropather::StateCost cost;
			cost.cost = tri->mDistance[i];
			cost.state = (void*)neighbor;

			if (!neighbor->mPassable)
				cost.cost = MAX_COST;

			adjacent->push_back(cost);
		}
	}

	void NavGraph::PrintStateInfo(void* state)
	{

	}

	bool NavGraph::Solve(const NavTriangle* start, const NavTriangle* end, std::vector<NavTriangle*>* path, float* cost) const
	{
		int rst = mPather->Solve((void*)start, (void*)end, (std::vector<void*>*)path, cost);
		if (MAX_COST <= *cost)
			return false;
		if (rst == micropather::MicroPather::SOLVED
			|| rst == micropather::MicroPather::START_END_SAME)
			return true;
		return false;
	}

	bool NavGraph::Solve(const Vector3& start, const Vector3& end, std::vector<Vector3>* path, float* cost, bool smoothPath) const
	{
		NavTriangle* triStart = GetTriangleByPoint(start);
		NavTriangle* triEnd = GetTriangleByPoint(end);
		std::vector<NavTriangle*> triPath;
		std::vector<NavEdge*> bounds;
		bool rst = Solve(triStart, triEnd, &triPath, cost);
		if (rst)
		{
			path->push_back(start);
			unsigned int nodeCount = (unsigned int)triPath.size();
			for (unsigned int i = 0; i < nodeCount; ++i)
			{
				NavTriangle* tri = triPath[i];
				path->push_back(tri->mCenter);
				NavTriangle* triNext = NULL;
				NavTriangle* triPre = NULL;

				if (i < (nodeCount - 1))
				{
					triNext = triPath[i + 1];
				}
				if (i > 0)
				{
					triPre = triPath[i - 1];
				}
				int boudsIndices[3] {0, 1, 2};
				unsigned int neighborCount = tri->mNeighbors.size();
				for (unsigned int j = 0; j < neighborCount; ++j)
				{
					if (tri->mNeighbors[j] == triNext)
					{
						path->push_back(tri->mEdgeCenter[j]);
					}
					if ((tri->mNeighbors[j] == triPre
						|| tri->mNeighbors[j] == triNext)
						&& smoothPath) // not pre not next;
					{
						int edgeIndex = tri->mShareEdgeIndices[j];
						boudsIndices[edgeIndex] = -1;
					}
				}
				for (unsigned int j = 0; j < 3; ++j)
				{
					int index = boudsIndices[j];
					if (index == -1)
						continue;
					int ptIndex0, ptIndex1;
					DecodeEdgeIndex(index, &ptIndex0, &ptIndex1);
					Vector3 pt0 = tri->mPoint[ptIndex0];
					Vector3 pt1 = tri->mPoint[ptIndex1];
					NavEdge* edge = new NavEdge();
					edge->mPoint[0] = pt0;
					edge->mPoint[1] = pt1;
					bounds.push_back(edge);
				}
			}
			path->push_back(end);

			if (smoothPath)
			{
				SmoothPath(path, bounds);
			}
		}
		for (unsigned int i = 0; i < bounds.size(); ++i)
		{
			SAFE_DELETE(bounds[i]);
		}
		return rst;
	}

	void NavGraph::ResetCost()
	{
		mPather->Reset();
	}

	bool NavGraph::LineTest(const Vector3& start, const Vector3& end, Vector3& hitPoint) const
	{
		Vector2 start2D(start.x, start.z);
		Vector2 end2D(end.x, end.z);

		Vector2 dir = end2D - start2D;
		float distance = dir.Length();
		dir.Normalize();
		Vector2 offset = dir * (-0.1f);

		for (unsigned int i = 0; i < mMesh->mBounds.size(); ++i)
		{
			NavEdge* edge = mMesh->mBounds[i];
			if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
				continue;

			Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
			Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
			Vector2 hitInfo;
			if (!NavPhysics::SegmentIntersectSegment(start2D, end2D, v0, v1, &hitInfo))
				continue;

			// 如果交点离边界高度差超过0.5米，则不算碰撞;
			float edgeLength = (v1 - v0).Length();
			float hitLength = (hitInfo - v0).Length();
			float edgeY = edge->mPoint[0].y + (edge->mPoint[1].y - edge->mPoint[0].y) * hitLength / (edgeLength + FLT_EPSILON);

			float lineLength = (hitInfo - start2D).Length();
			float lineY = start.y + (end.y - start.y) * lineLength / (distance + FLT_EPSILON);

			if (lineY > edgeY + 0.5f || lineY < edgeY - 0.5f)
				continue;

			hitPoint.Set(hitInfo.x + offset.x, start.y, hitInfo.y + offset.y);
			return true;
		}
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			for (unsigned int j = 0; j < gate->mBounds.size(); ++j)
			{
				NavEdge* edge = gate->mBounds[j];
				if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
					continue;

				Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
				Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
				Vector2 hitInfo;
				if (!NavPhysics::SegmentIntersectSegment(start2D, end2D, v0, v1, &hitInfo))
					continue;

				// 如果交点离边界高度差超过0.5米，则不算碰撞;
				float edgeLength = (v1 - v0).Length();
				float hitLength = (hitInfo - v0).Length();
				float edgeY = edge->mPoint[0].y + (edge->mPoint[1].y - edge->mPoint[0].y) * hitLength / (edgeLength + FLT_EPSILON);

				float lineLength = (hitInfo - start2D).Length();
				float lineY = start.y + (end.y - start.y) * lineLength / (distance + FLT_EPSILON);

				if (lineY > edgeY + 0.5f || lineY < edgeY - 0.5f)
					continue;

				hitPoint.Set(hitInfo.x + offset.x, start.y, hitInfo.y + offset.y);
				return true;
			}
		}
		return false;
	}

	bool NavGraph::LineTest(const Vector3& start, const Vector3& end, Vector3& hitPoint, Vector3& edgePoint0, Vector3& edgePoint1) const
	{
		Vector2 start2D(start.x, start.z);
		Vector2 end2D(end.x, end.z);

		Vector2 dir = end2D - start2D;
		float distance = dir.Length();
		dir.Normalize();
		Vector2 offset = dir * (-0.1f);

		for (unsigned int i = 0; i < mMesh->mBounds.size(); ++i)
		{
			NavEdge* edge = mMesh->mBounds[i];
			if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
				continue;
			Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
			Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
			Vector2 hitInfo;
			if (!NavPhysics::SegmentIntersectSegment(start2D, end2D, v0, v1, &hitInfo))
				continue;

			// 如果交点离边界高度差超过0.5米，则不算碰撞;
			float edgeLength = (v1 - v0).Length();
			float hitLength = (hitInfo - v0).Length();
			float edgeY = edge->mPoint[0].y + (edge->mPoint[1].y - edge->mPoint[0].y) * hitLength / (edgeLength + FLT_EPSILON);

			float lineLength = (hitInfo - start2D).Length();
			float lineY = start.y + (end.y - start.y) * lineLength / (distance + FLT_EPSILON);

			if (lineY > edgeY + 0.5f || lineY < edgeY - 0.5f)
				continue;

			edgePoint0 = edge->mPoint[0];
			edgePoint1 = edge->mPoint[1];
			hitPoint.Set(hitInfo.x + offset.x, start.y, hitInfo.y + offset.y);
			return true;
		}
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			for (unsigned int j = 0; j < gate->mBounds.size(); ++j)
			{
				NavEdge* edge = gate->mBounds[j];
				if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
					continue;
				Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
				Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
				Vector2 hitInfo;
				if (!NavPhysics::SegmentIntersectSegment(start2D, end2D, v0, v1, &hitInfo))
					continue;

				// 如果交点离边界高度差超过0.5米，则不算碰撞;
				float edgeLength = (v1 - v0).Length();
				float hitLength = (hitInfo - v0).Length();
				float edgeY = edge->mPoint[0].y + (edge->mPoint[1].y - edge->mPoint[0].y) * hitLength / (edgeLength + FLT_EPSILON);

				float lineLength = (hitInfo - start2D).Length();
				float lineY = start.y + (end.y - start.y) * lineLength / (distance + FLT_EPSILON);

				if (lineY > edgeY + 0.5f || lineY < edgeY - 0.5f)
					continue;

				edgePoint0 = edge->mPoint[0];
				edgePoint1 = edge->mPoint[1];
				hitPoint.Set(hitInfo.x + offset.x, start.y, hitInfo.y + offset.y);
				return true;
			}
		}
		return false;
	}

	bool NavGraph::IsLineTest(const Vector3& start, const Vector3& end) const
	{
		Vector2 start2D(start.x, start.z);
		Vector2 end2D(end.x, end.z);

		Vector2 dir = end2D - start2D;
		float distance = dir.Length();
		dir.Normalize();

		for (unsigned int i = 0; i < mMesh->mBounds.size(); ++i)
		{
			NavEdge* edge = mMesh->mBounds[i];
			if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
				continue;
			Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
			Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
			Vector2 hitInfo;
			if (!NavPhysics::SegmentIntersectSegment(start2D, end2D, v0, v1, &hitInfo))
				continue;

			// 如果交点离边界高度差超过0.5米，则不算碰撞;
			float edgeLength = (v1 - v0).Length();
			float hitLength = (hitInfo - v0).Length();
			float edgeY = edge->mPoint[0].y + (edge->mPoint[1].y - edge->mPoint[0].y) * hitLength / (edgeLength + FLT_EPSILON);

			float lineLength = (hitInfo - start2D).Length();
			float lineY = start.y + (end.y - start.y) * lineLength / (distance + FLT_EPSILON);

			if (lineY > edgeY + 0.5f || lineY < edgeY - 0.5f)
				continue;

			return true;
		}
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			for (unsigned int j = 0; j < gate->mBounds.size(); ++j)
			{
				NavEdge* edge = gate->mBounds[j];
				if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
					continue;
				Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
				Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
				Vector2 hitInfo;
				if (!NavPhysics::SegmentIntersectSegment(start2D, end2D, v0, v1, &hitInfo))
					continue;

				// 如果交点离边界高度差超过0.5米，则不算碰撞;
				float edgeLength = (v1 - v0).Length();
				float hitLength = (hitInfo - v0).Length();
				float edgeY = edge->mPoint[0].y + (edge->mPoint[1].y - edge->mPoint[0].y) * hitLength / (edgeLength + FLT_EPSILON);

				float lineLength = (hitInfo - start2D).Length();
				float lineY = start.y + (end.y - start.y) * lineLength / (distance + FLT_EPSILON);

				if (lineY > edgeY + 0.5f || lineY < edgeY - 0.5f)
					continue;

				return true;
			}
		}
		return false;
	}

	bool NavGraph::IsLineTest_Inner(const Vector3& start, const Vector3& end, const std::vector<NavEdge*>& bounds) const
	{
		Vector2 start2D(start.x, start.z);
		Vector2 end2D(end.x, end.z);

		Vector2 dir = end2D - start2D;
		float distance = dir.Length();
		dir.Normalize();

		for (unsigned int i = 0; i < bounds.size(); ++i)
		{
			NavEdge* edge = bounds[i];
			if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
				continue;
			Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
			Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
			Vector2 e01 = v1 - v0;
			e01.Normalize();
			v0 -= (e01 * 0.01f);
			v1 += (e01 * 0.01f);
			if (!NavPhysics::IsSegmentsInterct(start2D, end2D, v0, v1))
				continue;

			return true;
		}
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			for (unsigned int j = 0; j < gate->mBounds.size(); ++j)
			{
				NavEdge* edge = gate->mBounds[j];
				if (!NavPhysics::SegmentAABBSegment2D(start, end, edge->mPoint[0], edge->mPoint[1]))
					continue;
				Vector2 v0(edge->mPoint[0].x, edge->mPoint[0].z);
				Vector2 v1(edge->mPoint[1].x, edge->mPoint[1].z);
				Vector2 e01 = v1 - v0;
				e01.Normalize();
				v0 -= (e01 * 0.01f);
				v1 += (e01 * 0.01f);
				if (!NavPhysics::IsSegmentsInterct(start2D, end2D, v0, v1))
					continue;

				return true;
			}
		}
		return false;
	}

	void NavGraph::SmoothPath(std::vector<Vector3>* path, const std::vector<NavEdge*>& bounds) const
	{
		unsigned int pathSize = (unsigned int)path->size();
		Vector3* oldPath = new Vector3[pathSize];
		memcpy(oldPath, &((*path)[0]), pathSize * sizeof(Vector3));

		path->clear();
		path->push_back(oldPath[0]);

		int endIndex = pathSize - 1;
		while (endIndex > 0)
		{
			Vector3 end = oldPath[endIndex];
			if (!mHeightmap->HasHeightmap(oldPath[0]))
			{
				if (oldPath[0] != (*path)[0])
				{
					path->push_back(oldPath[0]);
				}
				pathSize = pathSize - 1;
				if (pathSize == 1)
				{
					path->push_back(end);
					break;
				}
				Vector3* temp = new Vector3[pathSize];
				memcpy(temp, &(oldPath[1]), pathSize * sizeof(Vector3));
				SAFE_DELETE_ARRAY(oldPath);
				oldPath = temp;
				endIndex = pathSize - 1;
				continue;
			}
			if (IsLineTest_Inner(oldPath[0], end, bounds))
			{
				--endIndex;
				continue;
			}

			path->push_back(end);

			pathSize = pathSize - endIndex;
			Vector3* temp = new Vector3[pathSize];
			memcpy(temp, &(oldPath[endIndex]), pathSize * sizeof(Vector3));
			SAFE_DELETE_ARRAY(oldPath);
			oldPath = temp;
			endIndex = pathSize - 1;
		}
		SAFE_DELETE_ARRAY(oldPath);
	}

	NavTriangle* NavGraph::GetTriangleByPoint(const Vector3& point) const
	{
		for (unsigned int i = 0; i < mMesh->mTriangles.size(); ++i)
		{
			NavTriangle* tri = mMesh->mTriangles[i];
			Vector3 v0 = tri->mPoint[0];
			Vector3 v1 = tri->mPoint[1];
			Vector3 v2 = tri->mPoint[2];

			if (!NavPhysics::TriangleAABBPoint2D(v0, v1, v2, point))
				continue;
			//NavPhysics::NavHit hitInfo;
			//Vector3 orig(point.x, 1000.0f, point.z);
			//if (!NavPhysics::RayIntersectTriangle(orig, Vector3::DOWN, v0, v1, v2, &hitInfo))
			//	continue;
			Vector2 v02D(v0.x, v0.z);
			Vector2 v12D(v1.x, v1.z);
			Vector2 v22D(v2.x, v2.z);
			Vector2 vp(point.x, point.z);
			if (!NavPhysics::IsPointInTriangle(v02D, v12D, v22D, vp))
				continue;
			return tri;
		}
		return NULL;
	}

	unsigned int NavGraph::GetSize()
	{
		unsigned int size = mMesh->GetSize();
		size += mHeightmap->GetSize();
		size += sizeof(unsigned int);
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			size += gate->GetSize();
		}
		return size;
	}

	unsigned int NavGraph::WriteTo(char* dest, unsigned int ptr)
	{
		ptr = mMesh->WriteTo(dest, ptr);
		ptr = mHeightmap->WriteTo(dest, ptr);
		unsigned int gateCount = (unsigned int)mGates.size();
		memcpy(dest + ptr, &gateCount, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			ptr = gate->WriteTo(dest, ptr);
		}

		return ptr;
	}

	unsigned int NavGraph::ReadFrom(char* src, unsigned int ptr)
	{
		for (unsigned int i = 0; i < mGates.size(); ++i)
		{
			NavGate* gate = mGates[i];
			SAFE_DELETE(gate);
		}
		mGates.clear();
		SAFE_DELETE(mMesh);
		SAFE_DELETE(mHeightmap);
		mMesh = new NavMesh();
		mHeightmap = new NavHeightmap();

		ptr = mMesh->ReadFrom(src, ptr);
		ptr = mHeightmap->ReadFrom(src, ptr);
		unsigned int gateCount = 0;
		memcpy(&gateCount, src + ptr, sizeof(unsigned int));
		ptr += sizeof(unsigned int);
		for (unsigned int i = 0; i < gateCount; ++i)
		{
			NavGate* gate = new NavGate(this);
			ptr = gate->ReadFrom(src, ptr);
			mGates.push_back(gate);
		}

		return ptr;
	}
}