#ifndef __NAV_PHYSICS_H__
#define __NAV_PHYSICS_H__

#include "NavInc.h"

class NavTriangle;

namespace NavPhysics
{
	struct NavHit
	{
		float distance;
		Vector3 hitPoint;
	};

	/* \功能:				判断三角形是否和射线相交;
	 * \参数:				射线起点;
	 * \参数:				射线方向，必须归一化;
	 * \参数:				三角形;
	 * \参数:				返回碰撞信息;
	 * \返回值：			true为相交，false为没相交;
	*/
	bool RayIntersectTriangle(const Vector3& orig, const Vector3& dir,
		const Vector3& v0, const Vector3& v1, const Vector3& v2,
		NavHit* hit);
}

#endif