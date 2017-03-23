#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include "Common.h"

class TriangleObject
{
private:
	struct Vertex {
		DirectX::XMFLOAT3 pos;
	};


public:
	TriangleObject();
	~TriangleObject();

private:
	ID3D12PipelineState*			m_pPSO;
	ID3D12GraphicsCommandList*		m_pCommandList;
	ID3D12RootSignature*			m_pRootSignature;
	ID3D12Resource*					m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_vertexBufferView;
};
