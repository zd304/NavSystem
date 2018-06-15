#ifndef __NAV_SYSTEM_DLL_H__
#define __NAV_SYSTEM_DLL_H__

#if defined (EXPORTBUILD)
#define _DLLExprot __declspec (dllexport)
#else
#define _DLLExprot __declspec (dllimport)
#endif

struct NAV_VEC3 { float x, y, z; };

struct NAV_VEC2 { float x, z; };

extern "C"
{
	_DLLExprot bool Create(const char* path);

	_DLLExprot void Release();

	_DLLExprot bool GetLayerCount(unsigned int* layerCount);

	_DLLExprot bool GetLayer(const NAV_VEC3* pos, unsigned int* layer);

	_DLLExprot bool GetNavHeight(const NAV_VEC3* pos, float* height, unsigned int* layer);

	_DLLExprot bool GetLayerHeight(const NAV_VEC3* pos, unsigned int layer, float* height);

	_DLLExprot bool LineCast(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos);

	_DLLExprot bool LineCastEdge(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos, NAV_VEC3* edgePoint0, NAV_VEC3* edgePoint1);

	_DLLExprot bool LineTest(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer);

	_DLLExprot bool RayCastLayer(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3* hitPos);

	_DLLExprot bool RayCastNav(const NAV_VEC3* start, const NAV_VEC3* end, NAV_VEC3* hitPos);

	_DLLExprot bool CalcLayerPath(const NAV_VEC3* start, const NAV_VEC3* end, unsigned int layer, NAV_VEC3** pathBuffer, unsigned int* pathNodeCount);

	_DLLExprot bool ReleaseLayerPath(NAV_VEC3** pathBuffer);

	_DLLExprot bool GetLayerTriangles(NAV_VEC3** verticesBuffer, unsigned int* verticesCount, unsigned int layer);

	_DLLExprot bool ReleaseLayerTriangles(NAV_VEC3** verticesBuffer);

	_DLLExprot bool GetLayerGateCount(unsigned int layer, unsigned int* gateCount);

	_DLLExprot bool IsLayerGatePassable(unsigned int layer, unsigned int gateIndex, bool* passable);

	_DLLExprot bool SetLayerGatePassable(unsigned int layer, unsigned int gateIndex, bool passable);

	_DLLExprot bool GetLayerCloseGates(unsigned int layer, NAV_VEC3** verticesBuffer, unsigned int* verticesCount);

	_DLLExprot bool ReleaseLayerCloseGates(NAV_VEC3** verticesBuffer);
}

#endif