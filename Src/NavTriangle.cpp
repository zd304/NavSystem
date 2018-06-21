#include "NavTriangle.h"

#ifdef _CHECK_LEAK
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
namespace Nav
{
	NavTriangle::NavTriangle()
	{
		mPassable = true;
	}

	NavTriangle::NavTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2)
	{
		mCenter.Set(0.0f, 0.0f, 0.0f);
		mCenter += v0;
		mCenter += v1;
		mCenter += v2;
		mCenter /= 3.0f;

		mPoint[0] = v0;
		mPoint[1] = v1;
		mPoint[2] = v2;
		mPassable = true;
	}

	void NavTriangle::GetBound(Vector3* min, Vector3* max)
	{
		if (min == NULL || max == NULL)
			return;
		max->Set(
			MaxFloat(MaxFloat(mPoint[0].x, mPoint[1].x), mPoint[2].x),
			MaxFloat(MaxFloat(mPoint[0].y, mPoint[1].y), mPoint[2].y),
			MaxFloat(MaxFloat(mPoint[0].z, mPoint[1].z), mPoint[2].z)
		);
		min->Set(
			MinFloat(MinFloat(mPoint[0].x, mPoint[1].x), mPoint[2].x),
			MinFloat(MinFloat(mPoint[0].y, mPoint[1].y), mPoint[2].y),
			MinFloat(MinFloat(mPoint[0].z, mPoint[1].z), mPoint[2].z)
		);
	}

	unsigned int NavTriangle::GetSize()
	{
		return sizeof(Vector3) * 3;
	}

	unsigned int NavTriangle::WriteTo(char* dest, unsigned int ptr)
	{
		memcpy(dest + ptr, &mPoint[0], sizeof(Vector3) * 3);
		ptr += sizeof(Vector3) * 3;
		return ptr;
	}

	unsigned int NavTriangle::ReadFrom(char* src, unsigned int ptr)
	{
		Vector3 v0, v1, v2;
		memcpy(v0.mem, &src[ptr], sizeof(Vector3));
		ptr += sizeof(Vector3);
		memcpy(v1.mem, &src[ptr], sizeof(Vector3));
		ptr += sizeof(Vector3);
		memcpy(v2.mem, &src[ptr], sizeof(Vector3));
		ptr += sizeof(Vector3);

		mCenter.Set(0.0f, 0.0f, 0.0f);
		mCenter += v0;
		mCenter += v1;
		mCenter += v2;
		mCenter /= 3.0f;

		mPoint[0] = v0;
		mPoint[1] = v1;
		mPoint[2] = v2;

		return ptr;
	}
}