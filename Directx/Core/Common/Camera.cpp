#include "Camera.h"


using namespace DirectX;

Camera::Camera()
{
	SetLens(0.25f * MathUtils::Pi, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPosition()const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 Camera::GetPosition3f()const
{
	return m_Position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
	m_ViewDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	m_Position = v;
	m_ViewDirty = true;
}

XMVECTOR Camera::GetRight()const
{
	return XMLoadFloat3(&m_Right);
}

XMFLOAT3 Camera::GetRight3f()const
{
	return m_Right;
}

XMVECTOR Camera::GetUp()const
{
	return XMLoadFloat3(&m_Up);
}

XMFLOAT3 Camera::GetUp3f()const
{
	return m_Up;
}

XMVECTOR Camera::GetLook()const
{
	return XMLoadFloat3(&m_Look);
}

XMFLOAT3 Camera::GetLook3f()const
{
	return m_Look;
}

float Camera::GetNearZ()const
{
	return m_NearZ;
}

float Camera::GetFarZ()const
{
	return m_FarZ;
}

float Camera::GetAspect()const
{
	return m_Aspect;
}

float Camera::GetFovY()const
{
	return m_FovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_NearZ);
}

float Camera::GetNearWindowWidth()const
{
	return m_Aspect * m_NearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return m_NearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return m_Aspect * m_FarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return m_FarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * mFarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, mFarZ);
	XMStoreFloat4x4(&m_Proj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);

	m_ViewDirty = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	m_ViewDirty = true;
}

XMMATRIX Camera::GetView()const
{
	assert(!m_ViewDirty);
	return XMLoadFloat4x4(&m_View);
}

XMMATRIX Camera::GetProj()const
{
	return XMLoadFloat4x4(&m_Proj);
}


XMFLOAT4X4 Camera::GetView4x4f()const
{
	assert(!m_ViewDirty);
	return m_View;
}

XMFLOAT4X4 Camera::GetProj4x4f()const
{
	return m_Proj;
}

