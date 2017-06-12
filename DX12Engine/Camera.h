#pragma once

#include "WindowClass.h"
#include <d3d12.h>
#include <DirectXMath.h>

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 initPosition, DirectX::XMFLOAT3 initLookAt);
	~Camera();

	D3D12_VIEWPORT GetViewport();
	D3D12_RECT GetScissorRect();

	DirectX::XMMATRIX GetVPMatrix();


private:

	D3D12_VIEWPORT m_viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT m_scissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	DirectX::XMFLOAT3	m_position;
	DirectX::XMFLOAT3	m_rotation;
	DirectX::XMFLOAT4X4	m_worldMatrix;


	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;
};
