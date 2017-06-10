#pragma once
#include "Common.h"
#include "D3dClass.h"
#include <DirectXMath.h>
#include "FrameBuffer.h"


class Plane
{
public:
	struct PlaneVertex
	{
		PlaneVertex(float x, float y, float z) : position(x, y, z) {}
		DirectX::XMFLOAT3 position;
	};

	struct WVPMatrixBufferStruct
	{
		DirectX::XMFLOAT4X4 wvpMat;
	};

public:
	Plane(FrameBuffer* pFrameBuffer);
	~Plane();

	void Update(Camera* camera);
	void Draw(FrameBuffer* pFrameBuffer, Camera* camera);


private:
	ID3D12RootSignature*					m_pRootSignature;

	ID3D12PipelineState*					m_pPSO;

	ID3D12Resource*							m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW				m_vertexBufferView;

	WVPMatrixBufferStruct					m_wvpMat;
	ID3D12Resource*							m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*									m_pWVPGPUAdress[g_cFrameBufferCount];
};

