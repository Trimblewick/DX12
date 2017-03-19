#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

#include "Common.h"

class Object
{
public:
	Object();
	~Object();

private:
	ID3D12PipelineState* m_pPSO;
	ID3D12RootSignature* m_pRS;

	struct Vertex
	{
		DirectX::XMFLOAT4 position;
	};
	
	ID3D12Resource* testVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};
