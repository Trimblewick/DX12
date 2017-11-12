#pragma once

#include "WindowClass.h"
#include "D3dClass.h"
#include <DirectXMath.h>
#include "Input.h"
#include <cmath>

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 initPosition, DirectX::XMFLOAT3 initLookAt);
	~Camera();


	D3D12_VIEWPORT		GetViewport();
	D3D12_RECT			GetScissorRect();

	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetVPMatrix();

	DirectX::XMFLOAT3	GetPosition();

	void				Update(Input * input, float dt, int iBackBufferIndex);
	void				BindCameraBuffer(int iCBVResourceSlot, ID3D12GraphicsCommandList* pCL, int iBackBufferIndex);

private:

	D3D12_VIEWPORT		m_viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT			m_scissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	DirectX::XMFLOAT3	m_position;
	DirectX::XMVECTOR	m_forward;
	DirectX::XMVECTOR	m_right;

	DirectX::XMVECTOR	m_fDefaultForward;
	DirectX::XMVECTOR	m_fDefaultRight;

	float m_fPitch;
	float m_fYaw;
	DirectX::XMMATRIX	m_rotMat;

	float				m_fForwardSpeed;
	float				m_fHorizontalSpeed;
	float				m_fVerticalSpeed;

	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;
	DirectX::XMMATRIX	m_transposedProjMatrix;
	
	struct BufferStruct
	{
		DirectX::XMFLOAT4X4 vpMatrix;
		DirectX::XMFLOAT3	position;
		float pad1;
		DirectX::XMFLOAT3	forward;
		float pad2;
		DirectX::XMFLOAT3	right;
		float pad3;
		DirectX::XMFLOAT3	up;
		float pad4;
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
	};

	
	BufferStruct		m_cameraBufferStruct;
	ID3D12Resource*		m_pCameraBuffer[g_iBackBufferCount];
	UINT8*				m_pCameraBufferAddress[g_iBackBufferCount];
};
