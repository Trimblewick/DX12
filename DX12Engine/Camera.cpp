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


	DirectX::XMMATRIX tempProjMat = DirectX::XMMatrixPerspectiveFovLH(45.0f*(3.14f / 180.0f), WindowClass::GetWidth() / WindowClass::GetHeight(), 0.1f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_projMatrix, tempProjMat);

	DirectX::XMVECTOR forward = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&initPosition), DirectX::XMLoadFloat3(&initLookAt));
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	DirectX::XMVECTOR right = DirectX::XMVector3Cross(forward, up);
	up = DirectX::XMVector3Cross(right, forward);
	/*
	WILL THIS BE NEEDED? ARE YOU RETARDED?
	DirectX::XMFLOAT3 sUp;
	DirectX::XMStoreFloat3(&sUp, up);
	if (sUp.y < 0.0f)
	{
		up = DirectX::XMVector3Cross(forward, right);
	}*/
	
	DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&initPosition), DirectX::XMLoadFloat3(&initLookAt), up);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, tempViewMatrix);
}

Camera::~Camera()
{
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
