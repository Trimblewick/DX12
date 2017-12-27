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

	m_fPitch = 0.0f;
	m_fYaw = 0.0f;

	DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&initPosition), m_forward, up);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, tempViewMatrix);

	m_rotMat = DirectX::XMMatrixIdentity();

	m_fForwardSpeed = 20.0f;
	m_fHorizontalSpeed = 20.0f;
	m_fVerticalSpeed = 20.0f;

	
	DirectX::XMFLOAT3 tempVec;
	DirectX::XMFLOAT4X4 tempMat;
	DirectX::XMStoreFloat3(&tempVec, m_forward);
	m_cameraBufferStruct.forward = tempVec;
	DirectX::XMStoreFloat3(&tempVec, m_right);
	m_cameraBufferStruct.right = tempVec;
	DirectX::XMStoreFloat3(&tempVec, up);
	m_cameraBufferStruct.up = tempVec;
	m_cameraBufferStruct.position = initPosition;
	DirectX::XMMATRIX transposedViewMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_viewMatrix));
	DirectX::XMStoreFloat4x4(&tempMat, transposedViewMatrix);
	m_cameraBufferStruct.viewMatrix = tempMat;
	m_transposedProjMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_projMatrix));
	DirectX::XMStoreFloat4x4(&tempMat, m_transposedProjMatrix);
	m_cameraBufferStruct.projMatrix = tempMat;
	DirectX::XMStoreFloat4x4(&tempMat, transposedViewMatrix * m_transposedProjMatrix);
	m_cameraBufferStruct.vpMatrix = tempMat;

	//create matrix buffer
	for (unsigned int i = 0; i < g_iBackBufferCount; ++i)
	{
		m_pCameraBuffer[i] = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_UPLOAD, 65536, D3D12_RESOURCE_STATE_GENERIC_READ, L"CAMERA UPLOAD HEAP" + i);
		//ZeroMemory(&m_cameraBufferStruct, sizeof(m_cameraBufferStruct));

		CD3DX12_RANGE readRange(0, 0);

		DxAssert(m_pCameraBuffer[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pCameraBufferAddress[i])), S_OK);

		memcpy(m_pCameraBufferAddress[i], &m_cameraBufferStruct, sizeof(m_cameraBufferStruct));
	}

}

Camera::~Camera()
{
	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_pCameraBuffer[i]);
	}
}

void Camera::Update(Input * pInput, float dt, int iBackBufferIndex)
{
	if (m_fPitch > 1.06f)
	{
		float fFrameDelta = pInput->GetMouseDelta().y * dt * 0.25f;
		if (fFrameDelta < 0)
			m_fPitch += fFrameDelta;
	}
	else if (m_fPitch < -1.06f)
	{
		float fFrameDelta = pInput->GetMouseDelta().y * dt * 0.25f;
		if (fFrameDelta > 0)
			m_fPitch += fFrameDelta;
	}
	else
	{
		m_fPitch += pInput->GetMouseDelta().y * dt * 0.25f;
	}
	m_fPitch = 0.0f;
	
	m_fYaw += pInput->GetMouseDelta().x * dt * 0.25f;

	m_rotMat = DirectX::XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, 0.0f);

	m_forward = DirectX::XMVector3TransformCoord(m_fDefaultForward, m_rotMat);
	m_right = DirectX::XMVector3TransformCoord(m_fDefaultRight, m_rotMat);

	float f = 0.0f;
	float r = 0.0f;
	float u = 0.0f;
	
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
	if (pInput->IsKeyDown(Input::SPACE))
	{
		u += dt * m_fVerticalSpeed;
	}
	if (pInput->IsKeyDown(Input::CTRL))
	{
		u -= dt * m_fVerticalSpeed;
	}

	m_position.x += DirectX::XMVectorGetX(m_forward) * f + DirectX::XMVectorGetX(m_right) * r;
	m_position.y += DirectX::XMVectorGetY(m_forward) * f + DirectX::XMVectorGetY(m_right) * r + u;
	m_position.z += DirectX::XMVectorGetZ(m_forward) * f + DirectX::XMVectorGetZ(m_right) * r;
	
	DirectX::XMVECTOR up = DirectX::XMVector3Cross(m_forward, m_right);
	DirectX::XMMATRIX newViewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_position), m_forward, up);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, newViewMatrix);



	DirectX::XMFLOAT3 tempVec;
	DirectX::XMFLOAT4X4 tempMat;
	DirectX::XMStoreFloat3(&tempVec, m_forward);
	m_cameraBufferStruct.forward = tempVec;
	DirectX::XMStoreFloat3(&tempVec, m_right);
	m_cameraBufferStruct.right = tempVec;
	DirectX::XMStoreFloat3(&tempVec, up);
	m_cameraBufferStruct.up = tempVec;
	m_cameraBufferStruct.position = m_position;
	DirectX::XMMATRIX transposedViewMatrix = DirectX::XMMatrixTranspose(newViewMatrix);
	DirectX::XMStoreFloat4x4(&tempMat, transposedViewMatrix);
	m_cameraBufferStruct.viewMatrix = tempMat;
	DirectX::XMStoreFloat4x4(&tempMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_viewMatrix) * DirectX::XMLoadFloat4x4(&m_projMatrix)));//transposedViewMatrix * m_transposedProjMatrix);
	m_cameraBufferStruct.vpMatrix = tempMat;


	memcpy(m_pCameraBufferAddress[iBackBufferIndex], &m_cameraBufferStruct, sizeof(m_cameraBufferStruct));

	return;
}

void Camera::BindCameraBuffer(int iCBVResourceSlot, ID3D12GraphicsCommandList * pCL, int iBackBufferIndex)
{
	pCL->RSSetViewports(1, &m_viewport);
	pCL->RSSetScissorRects(1, &m_scissorRect);

	pCL->SetComputeRootConstantBufferView(iCBVResourceSlot, m_pCameraBuffer[iBackBufferIndex]->GetGPUVirtualAddress());
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
