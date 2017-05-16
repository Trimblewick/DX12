#include "Camera.h"



Camera::Camera()
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
	DirectX::XMStoreFloat4x4(&m_dxProjMatrix, tempProjMat);

	DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
	DirectX::XMFLOAT4 lookAt = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 upDir = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	
	DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&pos), DirectX::XMLoadFloat4(&lookAt), DirectX::XMLoadFloat4(&upDir));
	DirectX::XMStoreFloat4x4(&m_dxViewMatrix, tempViewMatrix);
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
	return DirectX::XMLoadFloat4x4(&m_dxViewMatrix) * DirectX::XMLoadFloat4x4(&m_dxProjMatrix);
}
