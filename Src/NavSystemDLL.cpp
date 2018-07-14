#define EXPORTBUILD

#include "NavSystemDLL.h"
#include "Nav.h"

static Nav::NavSystem* navSystem = NULL;

extern "C"
{
	bool Nav_Create(const char* path)
	{
		navSystem = new Nav::NavSystem();
		if (!navSystem->LoadFromFile(path))
		{
			SAFE_DELETE(navSystem);
			return false;
		}
		return true;
	}

	bool Nav_CreateW(const unsigned char* path, unsigned int len)
	{
		navSystem = new Nav::NavSystem();

		unsigned long pathLen = len / (sizeof(wchar_t) / sizeof(unsigned char));
		wchar_t* pathData = new wchar_t[pathLen];
		memset(pathData, 0, sizeof(wchar_t) * pathLen);
		memcpy(pathData, path, len);

		if (!navSystem->LoadFromFileW(pathData))
		{
			SAFE_DELETE(navSystem);
			SAFE_DELETE_ARRAY(pathData);
			return false;
		}
		SAFE_DELETE_ARRAY(pathData);
		return true;
	}

	bool Nav_CreateFromMemory(char* data)
	{
		navSystem = new Nav::NavSystem();

		if (!navSystem->LoadFromMemory(data))
		{
			SAFE_DELETE(navSystem);
			return false;
		}
		return true;
	}

	void Nav_Release()
	{
		SAFE_DELETE(navSystem);
	}

	bool Nav_GetLayerCount(unsigned int* layerCount)
	{
		if (!navSystem) return false;
		(*layerCount) = navSystem->GetGraphCount();
		return true;
	}

	bool Nav_GetLayer(const NAV_VEC3* pos, unsigned int* layer)
	{
		if (!navSystem) return false;
		const Nav::Vector3 vPos((float*)pos);
		for (unsigned int i = 0; i < navSystem->GetGraphCount(); ++i)
		{
			Nav::NavGraph* graph = navSystem->GetGraphByID(i);
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

	bool Nav_GetNavHeight(const NAV_VEC3* pos, float* outHeight, unsigned int* layer)
	{
		if (!navSystem) return false;
		float h = FLT_MIN;
		bool rst = false;
		const Nav::Vector3 vPos((float*)pos);
		for (unsigned int i = 0; i < navSystem->GetGraphCount(); ++i)
		{
			Nav::NavGraph* graph = navSystem->GetGraphByID(i);
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

	bool Nav_GetLayerHeight(const NAV_VEC3* pos, unsigned int layer, float* outHeight)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Nav::Vector3 vPos((float*)pos);
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

	bool Nav_LineCast(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Nav::Vector3 vStart((float*)start);
		const Nav::Vector3 vEnd((float*)end);
		Nav::Vector3 hitPoint;
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

	bool Nav_LineCastEdge(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos, NAV_VEC3* edgePoint0, NAV_VEC3* edgePoint1)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Nav::Vector3 vStart((float*)start);
		const Nav::Vector3 vEnd((float*)end);
		Nav::Vector3 hitPoint;
		Nav::Vector3 ep0, ep1;
		if (graph->LineTest(vStart, vEnd, hitPoint, ep0, ep1))
		{
			hitPos->x = hitPoint.x;
			hitPos->y = hitPoint.y;
			hitPos->z = hitPoint.z;
			if (edgePoint0)
			{
				(*edgePoint0).x = ep0.x;
				(*edgePoint0).y = ep0.y;
				(*edgePoint0).z = ep0.z;
			}
			if (edgePoint1)
			{
				(*edgePoint1).x = ep1.x;
				(*edgePoint1).y = ep1.y;
				(*edgePoint1).z = ep1.z;
			}
			return true;
		}
		hitPos->x = end->x;
		hitPos->y = end->y;
		hitPos->z = end->z;
		return false;
	}

	bool Nav_LineTest(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		const Nav::Vector3 vStart((float*)start);
		const Nav::Vector3 vEnd((float*)end);
		return graph->IsLineTest(vStart, vEnd);
	}

	bool Nav_RayCastLayer(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		const Nav::Vector3 vStart((float*)start);
		const Nav::Vector3 vEnd((float*)end);

		NavPhysics::NavHit hit;
		for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
		{
			Nav::NavTriangle* tri = graph->mMesh->mTriangles[i];
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

	bool Nav_RayCastNav(const NAV_VEC3* start, const NAV_VEC3* end, NAV_VEC3* hitPos)
	{
		if (!navSystem) return false;

		const Nav::Vector3 vStart((float*)start);
		const Nav::Vector3 vEnd((float*)end);
		NavPhysics::NavHit hit;

		unsigned int layerCount = (unsigned int)navSystem->GetGraphCount();
		for (unsigned int layer = 0; layer < layerCount; ++layer)
		{
			Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
			for (size_t i = 0; i < graph->mMesh->mTriangles.size(); ++i)
			{
				Nav::NavTriangle* tri = graph->mMesh->mTriangles[i];
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

	bool Nav_CalcLayerPath(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3** pathBuffer, unsigned int* pathNodeCount)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh || !graph->mHeightmap) return false;

		const Nav::Vector3 vStart((float*)start);
		const Nav::Vector3 vEnd((float*)end);

		std::vector<Nav::Vector3> path;
		float cost;
		if (graph->Solve(vStart, vEnd, &path, &cost))
		{
			*pathBuffer = new NAV_VEC3[path.size()];
			for (size_t i = 0; i < path.size(); ++i)
			{
				Nav::Vector3& v = path[i];
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

	bool Nav_ReleaseLayerPath(NAV_VEC3** pathBuffer)
	{
		SAFE_DELETE_ARRAY(*pathBuffer);
		return true;
	}

	bool Nav_GetLayerTriangles(NAV_VEC3** verticesBuffer, unsigned int* verticesCount, unsigned int layer)
	{
		if (!verticesBuffer)
			return false;
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		unsigned int faceCount = (unsigned int)graph->mMesh->mTriangles.size();
		(*verticesBuffer) = new NAV_VEC3[faceCount * 3];
		(*verticesCount) = faceCount * 3;

		unsigned int index = -1;
		for (unsigned int i = 0; i < faceCount; ++i)
		{
			Nav::NavTriangle* tri = graph->mMesh->mTriangles[i];
			for (unsigned int j = 0; j < 3; ++j)
			{
				Nav::Vector3 v = tri->mPoint[j];

				NAV_VEC3* nv = &(*verticesBuffer)[++index];
				nv->x = v.x;
				nv->y = v.y;
				nv->z = v.z;
			}
		}
		return true;
	}

	bool Nav_ReleaseLayerTriangles(NAV_VEC3** verticesBuffer)
	{
		if (!verticesBuffer)
			return false;
		SAFE_DELETE_ARRAY(*verticesBuffer);
		return true;
	}

	bool Nav_GetLayerGateCount(unsigned int layer, unsigned int* gateCount)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		(*gateCount) = (unsigned int)graph->mGates.size();
		return true;
	}

	bool Nav_IsLayerGatePassable(unsigned int layer, unsigned int gateIndex, bool* passable)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		if (gateIndex >= (unsigned int)graph->mGates.size())
			return false;

		Nav::NavGate* gate = graph->mGates[gateIndex];
		if (!gate) return false;
		(*passable) = gate->mPassable;
		return true;
	}

	bool Nav_SetLayerGatePassable(unsigned int layer, unsigned int gateIndex, bool passable)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph) return false;

		if (gateIndex >= (unsigned int)graph->mGates.size())
			return false;

		Nav::NavGate* gate = graph->mGates[gateIndex];
		if (!gate) return false;

		gate->SwitchPassable(passable);
		return true;
	}

	bool Nav_GetLayerCloseGates(unsigned int layer, NAV_VEC3** verticesBuffer, unsigned int* vcount)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		unsigned int verticesCount = 0;
		for (size_t i = 0; i < graph->mGates.size(); ++i)
		{
			Nav::NavGate* gate = graph->mGates[i];
			if (gate->mPassable)
				continue;
			verticesCount += ((unsigned int)gate->mTriIndices.size() * 3);
		}

		(*vcount) = verticesCount;
		(*verticesBuffer) = new NAV_VEC3[verticesCount];
		int index = -1;
		for (size_t i = 0; i < graph->mGates.size(); ++i)
		{
			Nav::NavGate* gate = graph->mGates[i];
			if (gate->mPassable)
				continue;
			for (size_t j = 0; j < gate->mTriIndices.size(); ++j)
			{
				unsigned int triIndex = gate->mTriIndices[j];
				if ((size_t)triIndex >= graph->mMesh->mTriangles.size())
					continue;
				Nav::NavTriangle* tri = graph->mMesh->mTriangles[triIndex];
				for (int k = 0; k < 3; ++k)
				{
					NAV_VEC3& v = (*verticesBuffer)[++index];
					v.x = tri->mPoint[k].x;
					v.y = tri->mPoint[k].y;
					v.z = tri->mPoint[k].z;
				}
			}
		}
		return true;
	}

	bool Nav_ReleaseLayerCloseGates(NAV_VEC3** verticesBuffer)
	{
		if (!verticesBuffer)
			return false;
		SAFE_DELETE_ARRAY(*verticesBuffer);
		return true;
	}

	bool Nav_GetLayerBound(unsigned int layer, NAV_VEC3* min, NAV_VEC3* max)
	{
		if (!navSystem) return false;
		Nav::NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

		graph->mMesh->GetBound((Nav::Vector3*)min, (Nav::Vector3*)max);

		return true;
	}

	bool Nav_GetNavBound(NAV_VEC3* min, NAV_VEC3* max)
	{
		if (!navSystem) return false;

		navSystem->GetBound((Nav::Vector3*)min, (Nav::Vector3*)max);

		return true;
	}
}