#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT3 initPosition, DirectX::XMFLOAT3 initLookAt)
{
	// Fill out the Viewport
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = WindowClass::GetWidth();
	m_viewport.Height = WindowClass::GetHeight();
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = WindowClass::GetWidth();
	m_scissorRect.bottom = WindowClass::GetHeight();

	m_fDefaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_fDefaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	m_position = initPosition;

	DirectX::XMMATRIX tempProjMat = DirectX::XMMatrixPerspectiveFovLH(45.0f*(3.14f / 180.0f), WindowClass::GetWidth() / WindowClass::GetHeight(), 0.1f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_projMatrix, tempProjMat);

	m_forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&initLookAt), DirectX::XMLoadFloat3(&m_position)));
	
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_right = DirectX::XMVector3Cross(up, m_forward);

	m_fPitch = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_fDefaultForward, m_forward));
	m_fYaw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_fDefaultRight, m_right));

	DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&initPosition), m_forward, up);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, tempViewMatrix);

	m_fForwardSpeed = 15.0f;
	m_fHorizontalSpeed = 10.0f;
}

Camera::~Camera()
{
	
}

void Camera::Update(Input * pInput, float dt)
{
	m_fPitch += pInput->GetMouseDelta().y * dt;
	m_fYaw += pInput->GetMouseDelta().x * dt;
	
	m_rotMat = DirectX::XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.0f);

	m_forward = DirectX::XMVector3TransformCoord(m_fDefaultForward, m_rotMat);
	m_right = DirectX::XMVector3TransformCoord(m_fDefaultRight, m_rotMat);

	float f = 0.0f;
	float r = 0.0f;
	
	if (pInput->IsKeyDown(Input::W))
	{
		f += dt * m_fForwardSpeed;
	}
	if (pInput->IsKeyDown(Input::S))
	{
		f -= dt * m_fForwardSpeed;
	}
	if (pInput->IsKeyDown(Input::D))
	{
		r += dt * m_fHorizontalSpeed;
	}
	if (pInput->IsKeyDown(Input::A))
	{
		r -= dt * m_fHorizontalSpeed;
	}

	m_position.x += DirectX::XMVectorGetX(m_forward) * f + DirectX::XMVectorGetX(m_right) * r;
	m_position.y += DirectX::XMVectorGetY(m_forward) * f + DirectX::XMVectorGetY(m_right) * r;
	m_position.z += DirectX::XMVectorGetZ(m_forward) * f + DirectX::XMVectorGetZ(m_right) * r;
	

	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_position), m_forward, DirectX::XMVector3Cross(m_forward, m_right)));
	return;
}

D3D12_VIEWPORT Camera::GetViewport()
{
	return m_viewport;
}

D3D12_RECT Camera::GetScissorRect()
{
	return m_scissorRect;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return DirectX::XMLoadFloat4x4(&m_viewMatrix);
}

DirectX::XMMATRIX Camera::GetVPMatrix()
{
	return DirectX::XMLoadFloat4x4(&m_viewMatrix) * DirectX::XMLoadFloat4x4(&m_projMatrix);
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return m_position;
}
