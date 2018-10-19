#include "Camera.h"

Camera::Camera(const Nav::Vector3& pos, const Nav::Vector3& forward, float fov, float aspect, float nearClip, float farClip)
{
	mPosition = pos;
	mForward = forward;
	UpdateView();
	SetProjection(fov, aspect, nearClip, farClip);
}

Camera::~Camera()
{

}

void Camera::SetPosition(const Nav::Vector3& pos)
{
	mPosition = pos;
	UpdateView();
}

void Camera::SetForward(const Nav::Vector3& forward)
{
	Nav::Vector3 f = forward;
	f.Normalize();

	float cos = Nav::Vector3::Vector3Dot(f, Nav::Vector3::UP);
	if (cos > 0.9f)
	{
		return;
	}
	cos = Nav::Vector3::Vector3Dot(f, Nav::Vector3::DOWN);
	if (cos > 0.9f)
	{
		return;
	}
	mForward = f;

	UpdateView();
}

Nav::Vector3 Camera::GetPosition()
{
	return mPosition;
}

Nav::Vector3 Camera::GetDir()
{
	return mForward;
}

Nav::Vector3 Camera::GetRight()
{
	Nav::Vector3 right;
	Nav::Vector3::Vector3Cross(right, Nav::Vector3::UP, mForward);
	right.Normalize();
	return right;
}

void Camera::SetProjection(float fov, float aspect, float nearClip, float farClip)
{
	mFov = fov;
	mAspect = aspect;
	mNearClip = nearClip;
	mFarClip = farClip;
	UpdateProjection();
}

void Camera::UpdateProjection()
{
	D3DXMatrixPerspectiveFovLH(&mProjection, mFov, mAspect, mNearClip, mFarClip);
}

void Camera::UpdateView()
{
	D3DXVECTOR3 eyePt(mPosition.x, mPosition.y, mPosition.z);
	D3DXVECTOR3 lookAt(eyePt.x + mForward.x, eyePt.y + mForward.y, eyePt.z + mForward.z);

	D3DXVECTOR3 right, up;
	D3DXVECTOR3 forward(mForward.x, mForward.y, mForward.z);
	D3DXVec3Cross(&right, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &forward);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Cross(&up, &forward, &right);
	D3DXVec3Normalize(&up, &up);

	D3DXMatrixLookAtLH(&mView, &eyePt, &lookAt, &up);
}

void Camera::Update(void* device)
{
	UpdateView();
	UpdateProjection();

	IDirect3DDevice9* pDevice = (IDirect3DDevice9*)device;
	pDevice->SetTransform(D3DTS_VIEW, &mView);
	pDevice->SetTransform(D3DTS_PROJECTION, &mProjection);
}