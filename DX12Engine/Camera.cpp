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

	m_position = initPosition;

	DirectX::XMMATRIX tempProjMat = DirectX::XMMatrixPerspectiveFovLH(45.0f*(3.14f / 180.0f), WindowClass::GetWidth() / WindowClass::GetHeight(), 0.1f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_projMatrix, tempProjMat);

	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&initLookAt), DirectX::XMLoadFloat3(&initPosition)));
	DirectX::XMStoreFloat3(&m_forward, forward);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	DirectX::XMVECTOR right = DirectX::XMVector3Cross(forward, up);
	up = DirectX::XMVector3Cross(right, forward);
	DirectX::XMStoreFloat3(&m_up, up);
	/*
	WILL THIS BE NEEDED? ARE YOU RETARDED?
	DirectX::XMFLOAT3 sUp;
	DirectX::XMStoreFloat3(&sUp, up);
	if (sUp.y < 0.0f)
	{
		up = DirectX::XMVector3Cross(forward, right);
	}*/
	
	DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&initPosition), forward, up);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, tempViewMatrix);
}

Camera::~Camera()
{
	
}

void Camera::Update(Input * input)
{
	DirectX::XMVECTOR worldRight = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(1, 0, 0));
	DirectX::XMVECTOR worldForward = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 1));

	DirectX::XMVECTOR rightVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&m_forward), DirectX::XMLoadFloat3(&m_up)));
	DirectX::XMFLOAT3 test;
	DirectX::XMStoreFloat3(&test, rightVec);
	
	float cosAngle = DirectX::XMVectorGetX(DirectX::XMVector3Dot(worldRight, rightVec));
	
	int sign = cosAngle / std::abs(cosAngle);
	float otherAngle = std::sqrt(1 - cosAngle * cosAngle) * sign;
	

	if (input->IsKeyDown(input->KEYS::LEFT_ARROW))
	{
		m_position.z += 0.01 * otherAngle;
		m_position.x += 0.01 * cosAngle;
	}
	if (input->IsKeyDown(input->KEYS::RIGHT_ARROW))
	{
		m_position.z -= 0.01 * otherAngle;
		m_position.x -= 0.01 * cosAngle;
	}
	if (input->IsKeyDown(input->KEYS::UP_ARROW))
	{
		m_position.z -= 0.01 * cosAngle;
		m_position.x += 0.01 * otherAngle;
	}
	if (input->IsKeyDown(input->KEYS::DOWN_ARROW))
	{
		m_position.z += 0.01 * cosAngle;
		m_position.x -= 0.01 * otherAngle;
	}


	DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_position), DirectX::XMLoadFloat3(&m_forward), DirectX::XMLoadFloat3(&m_up));
	DirectX::XMStoreFloat4x4(&m_viewMatrix, tempViewMatrix);

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

DirectX::XMMATRIX Camera::GetVPMatrix()
{
	return DirectX::XMLoadFloat4x4(&m_viewMatrix) * DirectX::XMLoadFloat4x4(&m_projMatrix);
}
