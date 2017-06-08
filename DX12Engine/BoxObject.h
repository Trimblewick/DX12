#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include "Common.h"
#include "D3dClass.h"
#include "Camera.h"
#include "FrameBuffer.h"


class BoxObject
{
private:
	struct Vertex 
	{
		Vertex(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), color(r, g, b, a) {}
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	};

	struct ConstantBufferWVP  
	{
		DirectX::XMFLOAT4X4 wvpMat;
	};

	unsigned int m_iConstantBufferWVPAlignedSize = (sizeof(ConstantBufferWVP) + 255 & ~255);

public:
	BoxObject(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 rot, FrameBuffer* pFrameBuffer);
	~BoxObject();

	void Update(Camera* cam);
	void Draw(FrameBuffer* pFrameBuffer, Camera* cam);


private:
	ID3D12PipelineState*				m_pPSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_psoDesc;

	ID3D12Resource*						m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_vertexBufferView;

	ID3D12Resource*						m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW				m_indexBufferView;


	ConstantBufferWVP					m_cbWVP;

	DirectX::XMFLOAT4X4					m_xmRotationMatrix;
	DirectX::XMFLOAT4X4					m_xmTranslationMatrix;
	DirectX::XMFLOAT4X4					m_xmWorldMatrix;
	
	ID3D12Resource*						m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*								m_pWVPGPUAdress[g_cFrameBufferCount];

	ID3D12RootSignature*				m_pRootSignature;


	int m_iNumCubeIndices;

};

