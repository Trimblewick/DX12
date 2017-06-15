#pragma once
#include "Common.h"
#include "D3dClass.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include <random>

class GrassBlades
{
private:
	struct Vertex 
	{
		Vertex() : position(0, 0, 0) {}
		Vertex(float x, float y, float z) : position(x, y, z) {}
		DirectX::XMFLOAT3 position;
	};

	struct WVPMatrixBufferStruct
	{
		DirectX::XMFLOAT4X4 wvpMat;
	};
public:
	GrassBlades(FrameBuffer* pFrameBuffer);
	~GrassBlades();

	void Update(Camera* camera);
	void Draw(FrameBuffer* pFrameBuffer, Camera* camera);

private:
	ID3D12PipelineState*				m_pPSO;

	ID3D12RootSignature*				m_pRootSignature;
	

	ID3D12Resource*						m_pGrassBladesListVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_grassBladesListVertexBufferView;
	ID3D12DescriptorHeap*				m_pGrassBladeInstancesDH;
	//ID3D12DescriptorHeap*				m_pGrassBladesTextureDH;


	WVPMatrixBufferStruct					m_wvpMat;
	ID3D12Resource*							m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*									m_pWVPGPUAdress[g_cFrameBufferCount];

};
