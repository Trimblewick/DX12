#pragma once
#include "Common.h"
#include "D3dClass.h"
#include <DirectXMath.h>
#include "FrameBuffer.h"


class Plane
{
public:
	Plane(FrameBuffer* pFrameBuffer);
	~Plane();

	void Draw(FrameBuffer* pFrameBuffer, Camera* camera);

private:
	ID3D12PipelineState*					m_pPSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC		m_psoDesc;

	ID3D12Resource*							m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW				m_vertexBufferView;

	ID3D12RootSignature*					m_pRootSignature;

};

