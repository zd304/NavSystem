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

	bool GetNavHeight(const NAV_VEC3* pos, float* outHeight)
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
				rst = true;
				break;
			}
			if (graph->mMesh->GetHeight(vPos, &height))
			{
				h = h > height ? h : height;
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

	bool CalcLayerPath(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3** pathBuffer)
	{
		if (!navSystem) return false;
		NavGraph* graph = navSystem->GetGraphByID(layer);
		if (!graph || !graph->mMesh) return false;

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
				NAV_VEC3* node = &((*pathBuffer)[i]);
				node->x = v.x;
				node->y = v.y;
				node->z = v.z;
			}
			return true;
		}
		return false;
	}

	bool ReleaseLayerPath(NAV_VEC3** pathBuffer)
	{
		SAFE_DELETE_ARRAY(*pathBuffer);
		return true;
	}
}