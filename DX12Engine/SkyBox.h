#pragma once

#include "Common.h"
#include <d3d12.h>
#include "D3dClass.h"
#include "Texture.h"
#include "d3dx12.h"
#include "Camera.h"


class SkyBox
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		Vertex(float x, float y, float z, float u, float v) : position(x, y, z), uv(u, v) {}
		Vertex() : position(0, 0, 0), uv(0, 0) {}
	};

	struct WVPMatrixBufferStruct
	{
		DirectX::XMFLOAT4X4	worldMatrix;
		DirectX::XMFLOAT4X4 wvpMat;
	};

public:
	SkyBox();
	~SkyBox();

	void CreateSphere(int iLatLines, int iLongLines);
	
	void Update(Camera* camera);
	void Draw(ID3D12GraphicsCommandList* pCL, Camera* camera);

private:
	ID3D12PipelineState*			m_pPSO;

	ID3D12RootSignature*			m_pRS;

	ID3D12Resource*					m_pBufferVetex;
	D3D12_VERTEX_BUFFER_VIEW		m_viewVertex;

	ID3D12Resource*					m_pBufferIndex;
	D3D12_INDEX_BUFFER_VIEW			m_viewIndex;

	ID3D12DescriptorHeap*			m_pDH;
	ID3D12Resource*					m_pBufferTexture;
	int								m_iDHsize;
	
	WVPMatrixBufferStruct			m_wvpMat;
	ID3D12Resource*					m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*							m_pWVPGPUAdress[g_cFrameBufferCount];

};

