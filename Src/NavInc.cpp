#include "NavInc.h"

namespace Nav
{
	float MinFloat(float f1, float f2)
	{
		return f1 < f2 ? f1 : f2;
	}

	float MaxFloat(float f1, float f2)
	{
		return f1 > f2 ? f1 : f2;
	}
}