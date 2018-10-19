#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "inc.h"

class Camera
{
public:
	Camera(const Nav::Vector3& pos, const Nav::Vector3& forward, float fov, float aspect, float nearClip, float farClip);
	~Camera();

	void SetProjection(float fov, float aspect, float nearClip, float farClip);

	void SetPosition(const Nav::Vector3& pos);

	void SetForward(const Nav::Vector3& forward);

	Nav::Vector3 GetPosition();

	Nav::Vector3 GetDir();

	Nav::Vector3 GetRight();

	void Update(void* device);
private:
	void UpdateView();
	void UpdateProjection();
protected:
	float mFov, mAspect, mNearClip, mFarClip;
	Nav::Vector3 mPosition;
	Nav::Vector3 mForward;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

#endif