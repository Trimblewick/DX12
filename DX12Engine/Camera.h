#pragma once

#include "WindowClass.h"
#include <d3d12.h>
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera();

	D3D12_VIEWPORT GetViewport();
	D3D12_RECT GetScissorRect();

	DirectX::XMMATRIX GetVPMatrix();


private:

	D3D12_VIEWPORT m_viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT m_scissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	DirectX::XMFLOAT4X4 m_dxViewMatrix;
	DirectX::XMFLOAT4X4 m_dxProjMatrix;
};
