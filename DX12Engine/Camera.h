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


	D3D12_VIEWPORT GetViewport();
	D3D12_RECT GetScissorRect();

	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetVPMatrix();

	DirectX::XMFLOAT3 GetPosition();

	void Update(Input * input, float dt);

private:

	D3D12_VIEWPORT m_viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT m_scissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	DirectX::XMFLOAT3	m_position;
	DirectX::XMVECTOR	m_forward;
	DirectX::XMVECTOR	m_right;

	DirectX::XMVECTOR	m_fDefaultForward;
	DirectX::XMVECTOR	m_fDefaultRight;

	float m_fPitch;
	float m_fYaw;

	float m_fForwardSpeed;
	float m_fHorizontalSpeed;
	float m_fVerticalSpeed;

	DirectX::XMMATRIX	m_rotMat;

	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;

};
