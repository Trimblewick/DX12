#pragma once

#include "WindowClass.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "Input.h"
#include <cmath>

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 initPosition, DirectX::XMFLOAT3 initLookAt);
	~Camera();

	void Update(Input* input);

	D3D12_VIEWPORT GetViewport();
	D3D12_RECT GetScissorRect();

	DirectX::XMMATRIX GetVPMatrix();


private:

	D3D12_VIEWPORT m_viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT m_scissorRect; // the area to draw in. pixels outside that area will not be drawn onto



	DirectX::XMFLOAT3	m_position;
	DirectX::XMFLOAT3	m_forward;
	DirectX::XMFLOAT3	m_up;


	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;
};
