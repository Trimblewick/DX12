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
	m_focusPoint = initLookAt;
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
	float offsX = 0;
	float offsY = 0;
	float offsZ = 0;

	
	
	if (input->IsKeyDown(Input::LEFT_ARROW))
	{
		if (!input->IsKeyDown(Input::RIGHT_ARROW))
		{
			DirectX::XMFLOAT3 distanceVec;
			DirectX::XMStoreFloat3(&distanceVec, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_focusPoint), DirectX::XMLoadFloat3(&m_position)));
			float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y + distanceVec.z * distanceVec.z);

			DirectX::XMVECTOR tempForward = DirectX::XMLoadFloat3(&m_forward);
			DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationY(0.001);//DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&m_up), 0.01));
			tempForward = DirectX::XMVector3Transform(tempForward, rotMat);
			DirectX::XMFLOAT3 p;
			DirectX::XMStoreFloat3(&p, tempForward);
			DirectX::XMFLOAT3 newPos = DirectX::XMFLOAT3(m_focusPoint.x - p.x * distance, m_focusPoint.y - p.y * distance, m_focusPoint.z - p.z*distance);
			offsX = newPos.x - m_position.x;
			offsY = newPos.y - m_position.y;
			offsZ = newPos.z - m_position.z;
			
		}
	}
	else if (input->IsKeyDown(Input::RIGHT_ARROW))
	{
		DirectX::XMFLOAT3 distanceVec;
		DirectX::XMStoreFloat3(&distanceVec, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_focusPoint), DirectX::XMLoadFloat3(&m_position)));
		float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y + distanceVec.z * distanceVec.z);

		DirectX::XMVECTOR tempForward = DirectX::XMLoadFloat3(&m_forward);
		DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationY(-0.001);//DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&m_up), 0.01));
		tempForward = DirectX::XMVector3Transform(tempForward, rotMat);
		DirectX::XMFLOAT3 p;
		DirectX::XMStoreFloat3(&p, tempForward);
		DirectX::XMFLOAT3 newPos = DirectX::XMFLOAT3(m_focusPoint.x - p.x * distance, m_focusPoint.y - p.y * distance, m_focusPoint.z - p.z*distance);
		offsX = newPos.x - m_position.x;
		offsY = newPos.y - m_position.y;
		offsZ = newPos.z - m_position.z;


	}
	if (input->IsKeyDown(Input::UP_ARROW))
	{	
		offsX += m_forward.x * 0.01;
		offsY += m_forward.y * 0.01;
		offsZ += m_forward.z * 0.01;
	}
	if (input->IsKeyDown(Input::DOWN_ARROW))
	{
		offsX -= m_forward.x * 0.01;
		offsY -= m_forward.y * 0.01;
		offsZ -= m_forward.z * 0.01;
	}
	if (input->IsKeyDown(Input::SPACE))
	{
		offsY += 0.01;
	}
	if (input->IsKeyDown(Input::CTRL))
	{
		offsY -= 0.01;
	}
	if (offsX != 0 || offsY != 0 || offsZ != 0)
	{
		//recalc point
		DirectX::XMStoreFloat3(&m_position, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_position.x + offsX, m_position.y + offsY, m_position.z + offsZ)));
		//recalc forward
		DirectX::XMStoreFloat3(&m_forward, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_focusPoint), DirectX::XMLoadFloat3(&m_position))));
		//recalc viewmatrix
		DirectX::XMMATRIX tempViewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_position), DirectX::XMLoadFloat3(&m_focusPoint), DirectX::XMLoadFloat3(&m_up));
		DirectX::XMStoreFloat4x4(&m_viewMatrix, tempViewMatrix);
	}
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
