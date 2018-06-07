#include "NavHeightmap.h"
#include "NavTriangle.h"
#include "NavMesh.h"
#include "NavPhysics.h"

float MinFloat(float f1, float f2)
{
	return f1 < f2 ? f1 : f2;
}

float MaxFloat(float f1, float f2)
{
	return f1 > f2 ? f1 : f2;
}

NavHeightmap::NavHeightmap()
: mMin(FLT_MAX, FLT_MAX),
mMax(FLT_MIN, FLT_MIN),
mCellSize(1.0f, 1.0f)
{
	mSizeX = 0;
	mSizeZ = 0;
	mCellPassability = NULL;
	mHeights = NULL;
}

NavHeightmap::~NavHeightmap()
{
	SAFE_DELETE_ARRAY(mCellPassability);
	SAFE_DELETE_ARRAY(mHeights);
}

void NavHeightmap::BuildHeightmap(const NavMesh* mesh)
{
	SAFE_DELETE_ARRAY(mCellPassability);
	SAFE_DELETE_ARRAY(mHeights);
	mMin = Vector2(FLT_MAX, FLT_MAX);
	mMax = Vector2(FLT_MIN, FLT_MIN);

	std::vector<Vector3> vertices;
	for (size_t i = 0; i < mesh->mTriangles.size(); ++i)
	{
		NavTriangle* tri = mesh->mTriangles[i];
		for (size_t j = 0; j < 3; ++j)
		{
			Vector3& pos = tri->mPoint[j];
			mMin.x = mMin.x > pos.x ? pos.x : mMin.x;
			mMin.y = mMin.y > pos.z ? pos.z : mMin.y;

			mMax.x = mMax.x < pos.x ? pos.x : mMax.x;
			mMax.y = mMax.y < pos.z ? pos.z : mMax.y;

			vertices.push_back(pos);
		}
	}

	mMin = mMin - mCellSize * 2.0f;
	mMax = mMax + mCellSize * 2.0f;

	mMin.x = floorf(mMin.x);
	mMin.y = floorf(mMin.y);
	mMax.x = ceilf(mMax.x);
	mMax.y = ceilf(mMax.y);

	Vector2 vSize = mMax - mMin;
	mSizeX = (int)(ceilf(vSize.x / mCellSize.x) + 0.5f);
	mSizeZ = (int)(ceilf(vSize.y / mCellSize.y) + 0.5f);

	Vector2* arrMap = new Vector2[(mSizeZ + 1) * (mSizeX + 1)];
	bool* arrMapMask = new bool[(mSizeZ + 1) * (mSizeX + 1)];
	mHeights = new float[(mSizeZ + 1) * (mSizeX + 1)];
	for (int j = 0; j < mSizeZ + 1; ++j)
	{
		for (int i = 0; i < mSizeX + 1; ++i)
		{
			int index = j * (mSizeX + 1) + i;
			arrMap[index] = mMin + Vector2(mCellSize.x * i, mCellSize.y * j);
			arrMapMask[index] = false;
			mHeights[index] = 0.0f;
		}
	}
	for (size_t i = 0; i + 2 < vertices.size(); i += 3)
	{
		Vector3 vVertex0 = vertices[i];
		Vector3 vVertex1 = vertices[i + 1];
		Vector3 vVertex2 = vertices[i + 2];

		Vector2 vSubMin;
		Vector2 vSubMax;
		vSubMin.x = MinFloat(MinFloat(vVertex0.x, vVertex1.x), vVertex2.x);
		vSubMin.y = MinFloat(MinFloat(vVertex0.z, vVertex1.z), vVertex2.z);

		int nGridStartIndexX = (int)(floorf((vSubMin.x - mMin.x) / mCellSize.x) - 0.5f);
		int nGridStartIndexY = (int)(floorf((vSubMin.y - mMin.y) / mCellSize.y) - 0.5f);

		vSubMax.x = MaxFloat(MaxFloat(vVertex0.x, vVertex1.x), vVertex2.x);
		vSubMax.y = MaxFloat(MaxFloat(vVertex0.z, vVertex1.z), vVertex2.z);

		int nGridEndIndexX = (int)(ceilf((vSubMax.x - mMin.x) / mCellSize.x) + 0.5f);
		int nGridEndIndexY = (int)(ceilf((vSubMax.y - mMin.y) / mCellSize.y) + 0.5f);

		for (int x = nGridStartIndexX; x <= nGridEndIndexX; ++x)
		{
			for (int y = nGridStartIndexY; y <= nGridEndIndexY; ++y)
			{
				if (!arrMapMask[y * (mSizeX + 1) + x])
				{
					Vector2 vPos = arrMap[y * (mSizeX + 1) + x];
					Vector3 orig(vPos.x, 100.0f, vPos.y);

					NavPhysics::NavHit hitInfo;
					bool bInside = NavPhysics::RayIntersectTriangle(orig, Vector3::DOWN, vVertex0, vVertex1, vVertex2, &hitInfo);
					if (bInside)
					{
						mHeights[y * (mSizeX + 1) + x] = hitInfo.hitPoint.y;
					}
					arrMapMask[y * (mSizeX + 1) + x] = bInside;
				}
			}
		}

		mCellPassability = new char[mSizeZ * mSizeX];
		for (int j = 0; j < mSizeZ; ++j)
		{
			for (int i = 0; i < mSizeX; ++i)
			{
				bool bValue = true;
				bValue = arrMapMask[j * (mSizeX + 1) + i]
					& arrMapMask[j * (mSizeX + 1) + i + 1]
					& arrMapMask[(j + 1) * (mSizeX + 1) + i]
					& arrMapMask[(j + 1) * (mSizeX + 1) + i + 1];

				mCellPassability[j * mSizeX + i] = bValue ? (char)1 : (char)0;
			}
		}
	}
}

void NavHeightmap::SetCellSize(const Vector2& size)
{
	if (size.x <= 0.0f || size.y <= 0.0f)
		return;
	mCellSize = size;
}

bool NavHeightmap::GetHeight(const Vector3& pos, float* height)
{
	if (pos.x < mMin.x || pos.z < mMin.y
		|| pos.x > mMax.x || pos.z > mMax.y)
	{
		return false;
	}
	Vector3 p = pos;
	p.x -= mMin.x;
	p.z -= mMin.y;
	int col = (int)floorf(p.x / mCellSize.x);
	int row = (int)floorf(p.z / mCellSize.y);

	if (mCellPassability[row * mSizeX + col] == 0)
		return false;

	float u = fmodf(p.x, mCellSize.x);
	float v = fmodf(p.z, mCellSize.y);

	int nextCol = (nextCol < mSizeX ? col + 1 : col);
	int nextRow = (nextRow < mSizeZ ? row + 1 : row);

	int countX = mSizeX + 1;
	float h0 = mHeights[row * countX + col];
	float h1 = mHeights[row * countX + nextCol];
	float h2 = mHeights[nextRow * countX + col];
	float h3 = mHeights[nextRow * countX + nextCol];

	float r0 = h0 + (h1 - h0) * u;
	float r1 = h2 + (h3 - h2) * u;

	float h = r0 + (r1 - r0) * v;

	if (h > pos.y + 5.0f)
		return false;
	*height = h;
	return true;
}

void NavHeightmap::SwitchCellPassability(const Vector3& v0, const Vector3& v1, const Vector3& v2, bool passable)
{
	Vector2 vSubMin;
	Vector2 vSubMax;
	vSubMin.x = MinFloat(MinFloat(v0.x, v1.x), v2.x);
	vSubMin.y = MinFloat(MinFloat(v0.z, v1.z), v2.z);

	int nGridStartIndexX = (int)(floorf((vSubMin.x - mMin.x) / mCellSize.x) - 0.5f);
	int nGridStartIndexY = (int)(floorf((vSubMin.y - mMin.y) / mCellSize.y) - 0.5f);

	vSubMax.x = MaxFloat(MaxFloat(v0.x, v1.x), v2.x);
	vSubMax.y = MaxFloat(MaxFloat(v0.z, v1.z), v2.z);

	int nGridEndIndexX = (int)(ceilf((vSubMax.x - mMin.x) / mCellSize.x) + 0.5f);
	int nGridEndIndexY = (int)(ceilf((vSubMax.y - mMin.y) / mCellSize.y) + 0.5f);
	for (int x = nGridStartIndexX; x <= nGridEndIndexX; ++x)
	{
		if (x + 1 > nGridStartIndexX)
			continue;
		for (int y = nGridStartIndexY; y <= nGridEndIndexY; ++y)
		{
			if (y + 1 > nGridStartIndexY)
				continue;
			if (passable)
				mCellPassability[y * mSizeX + x] = (char)1;
			else
				mCellPassability[y * mSizeX + x] = (char)0;
		}
	}
}

unsigned int NavHeightmap::GetSize()
{
	unsigned int size = sizeof(Vector2)* 3;
	size += sizeof(int)* 2;
	size += sizeof(char)* mSizeX * mSizeZ;
	size += sizeof(float)* (mSizeX + 1) * (mSizeZ + 1);
	return size;
}

unsigned int NavHeightmap::WriteTo(char* dest, unsigned int ptr)
{
	memcpy(dest + ptr, &mMin, sizeof(Vector2));
	ptr += sizeof(Vector2);
	memcpy(dest + ptr, &mMax, sizeof(Vector2));
	ptr += sizeof(Vector2);
	memcpy(dest + ptr, &mCellSize, sizeof(Vector2));
	ptr += sizeof(Vector2);

	memcpy(dest + ptr, &mSizeX, sizeof(int));
	ptr += sizeof(int);
	memcpy(dest + ptr, &mSizeZ, sizeof(int));
	ptr += sizeof(int);

	memcpy(dest + ptr, mCellPassability, sizeof(char)* mSizeX * mSizeZ);
	ptr += (sizeof(char)* mSizeX * mSizeZ);
	memcpy(dest + ptr, mHeights, sizeof(float)* (mSizeX + 1) * (mSizeZ + 1));
	ptr += (sizeof(float)* (mSizeX + 1) * (mSizeZ + 1));

	return ptr;
}

unsigned int NavHeightmap::ReadFrom(char* src, unsigned int ptr)
{
	SAFE_DELETE_ARRAY(mCellPassability);
	SAFE_DELETE_ARRAY(mHeights);

	memcpy(mMin.mem, src + ptr, sizeof(Vector2));
	ptr += sizeof(Vector2);
	memcpy(mMax.mem, src + ptr, sizeof(Vector2));
	ptr += sizeof(Vector2);
	memcpy(mCellSize.mem, src + ptr, sizeof(Vector2));
	ptr += sizeof(Vector2);

	memcpy(&mSizeX, src + ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(&mSizeZ, src + ptr, sizeof(int));
	ptr += sizeof(int);

	mCellPassability = new char[mSizeX * mSizeZ];
	mHeights = new float[(mSizeX + 1) * (mSizeZ + 1)];
	memcpy(mCellPassability, src + ptr, sizeof(char)* mSizeX * mSizeZ);
	ptr += (sizeof(char)* mSizeX * mSizeZ);
	memcpy(mHeights, src + ptr, sizeof(float)* (mSizeX + 1) * (mSizeZ + 1));
	ptr += (sizeof(float)* (mSizeX + 1) * (mSizeZ + 1));

	return ptr;
}