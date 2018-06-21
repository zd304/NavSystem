#ifndef __NAV_PHYSICS_H__
#define __NAV_PHYSICS_H__

#include "NavInc.h"

class NavTriangle;

namespace NavPhysics
{
	struct NavHit
	{
		float distance;
		Nav::Vector3 hitPoint;
	};

	/************************************************************************
	 * 功能:				判断三角形是否和射线相交;
	 * 参数:				射线起点;
	 * 参数:				射线方向，必须归一化;
	 * 参数:				三角形;
	 * 参数:				返回碰撞信息;
	 * 返回值：				true为相交，false为没相交;
	*************************************************************************/
	bool RayIntersectTriangle(const Nav::Vector3& orig, const Nav::Vector3& dir,
		const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2,
		NavHit* hit);

	/************************************************************************
	 * 功能:				判断线段是否和射线相交;
	 * 参数:				射线起点;
	 * 参数:				射线方向，必须归一化;
	 * 参数:				线段;
	 * 参数:				返回碰撞信息;
	 * 返回值：				true为相交，false为没相交;
	*************************************************************************/
	bool RayIntersectSegment2D(const Nav::Vector2& orig, const Nav::Vector2& dir,
		const Nav::Vector2& lineBegin, const Nav::Vector2& lineEnd,
		NavHit* hit);

	/************************************************************************
	 * 功能:				判断二维点是否和三角形相交，做三角形位置检测时候可以先调用这个方法，减少射线和三角形相交计算;
	 * 参数:				三角形三个点;
	 * 参数:				二维点;
	 * 返回值：				true为相交，false为没相交;
	*************************************************************************/
	bool TriangleAABBPoint2D(const Nav::Vector2& v0, const Nav::Vector2& v1, const Nav::Vector2& v2,
		const Nav::Vector2& point2D);

	/************************************************************************
	* 功能:					TriangleIntersect2DPoint的3D参数版本;
	* 参数:					三角形三个点;
	* 参数:					二维点;
	* 返回值：				true为相交，false为没相交;
	*************************************************************************/
	bool TriangleAABBPoint2D(const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2,
		const Nav::Vector3& point2D);

	/************************************************************************
	* 功能:					两条线段的AABB做相交检测;
	* 参数:					线段1;
	* 参数:					线段2;
	* 返回值：				true为相交，false为没相交;
	*************************************************************************/
	bool SegmentAABBSegment2D(const Nav::Vector3& line1Begin, const Nav::Vector3& line1End,
		const Nav::Vector3& line2Begin, const Nav::Vector3& line2End);

	bool IsPointInTriangle(const Nav::Vector2& v0, const Nav::Vector2& v1, const Nav::Vector2& v2, const Nav::Vector2& point);

	bool IsPointInTriangle2D(const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2, const Nav::Vector3& point);

	bool IsSegmentsInterct(const Nav::Vector2& line1Begin, const Nav::Vector2& line1End,
		const Nav::Vector2& line2Begin, const Nav::Vector2& line2End);

	bool SegmentIntersectSegment(const Nav::Vector2& line1Begin, const Nav::Vector2& line1End,
		const Nav::Vector2& line2Begin, const Nav::Vector2& line2End,
		Nav::Vector2* hitPoint);

	float CalcTriangleArea2D(const Nav::Vector3& v0, const Nav::Vector3& v1, const Nav::Vector3& v2);
}

#endif