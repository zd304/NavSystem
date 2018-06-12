#define EXPORTBUILD

#include "NavSystemDLL.h"
#include "Nav.h"

static NavSystem* navSystem = NULL;

extern "C"
{
	bool Create(const char* path)
	{
		navSystem = new NavSystem();
		navSystem->LoadFromFile(path);
		return true;
	}

	void Release()
	{
		SAFE_DELETE(navSystem);
	}

	bool GetLayerCount(unsigned int* layerCount)
	{
		if (!navSystem) return false;
		(*layerCount) = navSystem->GetGraphCount();
		return true;
	}

	bool GetLayer(const NAV_VEC3* pos, unsigned int* layer)
	{
		if (!navSystem) return false;
		const Vector3 vPos((float*)pos);
		for (unsigned int i = 0; i < navSystem->GetGraphCount(); ++i)
		{
			NavGraph* graph = navSystem->GetGraphByID(i);
			float height = 0.0f;
			if (graph->mHeightmap->GetHeight(vPos, &height))
			{
				(*layer) = i;
				return true;
			}
			if (graph->mMesh->GetHeight(vPos, &height))
			{
				(*layer) = i;
				return true;
			}
		}
		return false;
	}

	bool GetNavHeight(const NAV_VEC3* pos, float* outHeight, unsigned int* layer)
	{
		if (!navSystem) return false;
		float h = FLT_MIN;
		bool rst = false;
		const Vector3 vPos((float*)pos);
		for (unsigned int i = 0; i < navSystem->GetGraphCount(); ++i)
		{
			NavGraph* graph = navSystem->GetGraphByID(i);
			float height = 0.0f;
			if (graph->mHeightmap->GetHeight(vPos, &height))
			{
				h = h > height ? h : height;
				(*layer) = i;
				rst = true;
				break;
			}
			if (graph->mMesh->GetHeight(vPos, &height))
			{
				h = h > height ? h : height;
				(*layer) = i;
				rst = true;
				break;
			}
		}
		if (rst)
		{
			(*outHeight) = h;
		}
		return rst;
	}

	bool GetLayerHeight(const NAV_VEC3* pos, unsigned int layer, float* outHeight)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Vector3 vPos((float*)pos);
		float height = 0.0f;
		if (graph->mHeightmap->GetHeight(vPos, &height))
		{
			(*outHeight) = height;
			return true;
		}
		if (graph->mMesh->GetHeight(vPos, &height))
		{
			(*outHeight) = height;
			return true;
		}
		return false;
	}

	bool LineCast(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Vector3 vStart((float*)start);
		const Vector3 vEnd((float*)end);
		Vector3 hitPoint;
		if (graph->LineTest(vStart, vEnd, hitPoint))
		{
			hitPos->x = hitPoint.x;
			hitPos->y = hitPoint.y;
			hitPos->z = hitPoint.z;
			return true;
		}
		hitPos->x = end->x;
		hitPos->y = end->y;
		hitPos->z = end->z;
		return false;
	}

	bool LineTest(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Vector3 vStart((float*)start);
		const Vector3 vEnd((float*)end);
		return graph->IsLineTest(vStart, vEnd);
	}

	bool RayCastLayer(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		const Vector3 vStart((float*)start);
		const Vector3 vEnd((float*)end);

		NavPhysics::NavHit hit;
		for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
		{
			NavTriangle* tri = graph->mMesh->mTriangles[i];
			if (!tri->mPassable)
				continue;
			if (NavPhysics::RayIntersectTriangle(vStart, vEnd, tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], &hit))
			{
				hitPos->x = hit.hitPoint.x;
				hitPos->y = hit.hitPoint.y;
				hitPos->z = hit.hitPoint.z;
				return true;
			}
		}
		return false;
	}

	bool RayCastNav(const NAV_VEC3* start, const NAV_VEC3* end, NAV_VEC3* hitPos)
	{
		if (!navSystem) return false;

		const Vector3 vStart((float*)start);
		const Vector3 vEnd((float*)end);
		NavPhysics::NavHit hit;

		unsigned int layerCount = (unsigned int)navSystem->GetGraphCount();
		for (unsigned int layer = 0; layer < layerCount; ++layer)
		{
			NavGraph* graph = navSystem->GetGraphByID(layer);
			for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
			{
				NavTriangle* tri = graph->mMesh->mTriangles[i];
				if (!tri->mPassable)
					continue;
				if (NavPhysics::RayIntersectTriangle(vStart, vEnd, tri->mPoint[0], tri->mPoint[1], tri->mPoint[2], &hit))
				{
					hitPos->x = hit.hitPoint.x;
					hitPos->y = hit.hitPoint.y;
					hitPos->z = hit.hitPoint.z;
					return true;
				}
			}
		}
		return false;
	}

	bool CalcLayerPath(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3** pathBuffer, unsigned int* pathNodeCount)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh || !graph->mHeightmap) return false;

		const Vector3 vStart((float*)start);
		const Vector3 vEnd((float*)end);

		std::vector<Vector3> path;
		float cost;
		if (graph->Solve(vStart, vEnd, &path, &cost))
		{
			*pathBuffer = new NAV_VEC3[path.size()];
			for (size_t i = 0; i < path.size(); ++i)
			{
				Vector3& v = path[i];
				float height = v.y + 0.5f;
				if (!graph->mHeightmap->GetHeight(v, &height))
				{
					if (!graph->mMesh->GetHeight(v, &height))
					{
						return false;
					}
				}
				

				NAV_VEC3* node = &((*pathBuffer)[i]);
				node->x = v.x;
				node->y = height;
				node->z = v.z;
			}
			(*pathNodeCount) = (unsigned int)path.size();
			return true;
		}
		return false;
	}

	bool ReleaseLayerPath(NAV_VEC3** pathBuffer)
	{
		SAFE_DELETE_ARRAY(*pathBuffer);
		return true;
	}

	bool GetLayerTriangles(NAV_VEC3** verticesBuffer, unsigned int* verticesCount, unsigned int layer)
	{
		if (!verticesBuffer)
			return false;
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		unsigned int faceCount = (unsigned int)graph->mMesh->mTriangles.size();
		(*verticesBuffer) = new NAV_VEC3[faceCount * 3];
		(*verticesCount) = faceCount * 3;

		unsigned int index = -1;
		for (unsigned int i = 0; i < faceCount; ++i)
		{
			NavTriangle* tri = graph->mMesh->mTriangles[i];
			for (unsigned int j = 0; j < 3; ++j)
			{
				Vector3 v = tri->mPoint[j];

				NAV_VEC3* nv = &(*verticesBuffer)[++index];
				nv->x = v.x;
				nv->y = v.y;
				nv->z = v.z;
			}
		}
		return true;
	}

	bool ReleaseLayerTriangles(NAV_VEC3** verticesBuffer)
	{
		if (!verticesBuffer)
			return false;
		SAFE_DELETE_ARRAY(*verticesBuffer);
		return true;
	}

	bool GetLayerGateCount(unsigned int layer, unsigned int* gateCount)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		(*gateCount) = (unsigned int)graph->mGates.size();
		return true;
	}

	bool IsLayerGatePassable(unsigned int layer, unsigned int gateIndex, bool* passable)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		if (gateIndex >= (unsigned int)graph->mGates.size())
			return false;

		NavGate* gate = graph->mGates[gateIndex];
		if (!gate) return false;
		(*passable) = gate->mPassable;
		return true;
	}

	bool SetLayerGatePassable(unsigned int layer, unsigned int gateIndex, bool passable)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		if (gateIndex >= (unsigned int)graph->mGates.size())
			return false;

		NavGate* gate = graph->mGates[gateIndex];
		if (!gate) return false;

		gate->SwitchPassable(passable);
		return true;
	}

	bool GetLayerCloseGates(unsigned int layer, NAV_VEC3** verticesBuffer, unsigned int* verticesCount)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		unsigned int verticesCount = 0;
		for (size_t i = 0; i < graph->mGates.size(); ++i)
		{
			NavGate* gate = graph->mGates[i];
			if (gate->mPassable)
				continue;
			verticesCount += ((unsigned int)gate->mTriIndices.size() * 3);
		}

		(*verticesBuffer) = new NAV_VEC3[verticesCount];
		for (size_t i = 0; i < graph->mGates.size(); ++i)
		{
			NavGate* gate = graph->mGates[i];
			if (gate->mPassable)
				continue;
			for (size_t j = 0; j < gate->mTriIndices.size(); ++j)
			{
				unsigned int index = gate->mTriIndices[j];
				if ((size_t)index >= graph->mMesh->mTriangles.size())
					continue;
				NavTriangle* tri = graph->mMesh->mTriangles[index];

			}
		}
	}
}