#ifndef __NAV_HEIGHTMAP_H__
#define __NAV_HEIGHTMAP_H__

#include "NavInc.h"

class NavMesh;

class NavHeightmap
{
public:
	NavHeightmap();

	~NavHeightmap();

	void BuildHeightmap(const NavMesh* mesh);

	void SetCellSize(const Vector2& size);

	bool GetHeight(const Vector3& pos, float* height);

	unsigned int GetSize();

	unsigned int WriteTo(char* dest, unsigned int ptr);

	unsigned int ReadFrom(char* src, unsigned int ptr);
private:
	Vector2 mMin;
	Vector2 mMax;
	Vector2 mCellSize;
	int mSizeX;
	int mSizeZ;
	char* mCellPassability;
	float* mHeights;
};

#endif
