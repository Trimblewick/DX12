#pragma once
#include "Common.h"
#include "D3dClass.h"
#include <DirectXMath.h>
#include "FrameBuffer.h"
#include "Texture.h"

class Plane
{
public:
	struct PlaneVertex
	{
		PlaneVertex() : position(0, 0, 0) {}
		PlaneVertex(float x, float y, float z, float u, float v) : position(x, y, z), uv(u, v) {}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
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

	ID3D12Resource*							m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW					m_indexBufferView;
	int										m_iNrOfIndices;

	WVPMatrixBufferStruct					m_wvpMat;
	ID3D12Resource*							m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*									m_pWVPGPUAdress[g_cFrameBufferCount];

	Texture*								m_pGrassTexture;
	ID3D12Resource*							m_pGrassTextureBuffer;
	ID3D12DescriptorHeap*					m_pTextureDH;
};

