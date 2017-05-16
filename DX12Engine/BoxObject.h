#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include "Common.h"
#include "D3dClass.h"
#include "PSOHandler.h"
#include "Camera.h"


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
	BoxObject(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 rot, PSOHandler* pPsoHandler);
	~BoxObject();

	void Update(Camera* cam);
	void Draw(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, Camera* cam);

	ID3D12CommandList* GetGraphicsCommandList();

private:
	ID3D12PipelineState*				_pPSO;
	ID3D12GraphicsCommandList*			m_pCommandList;
	ID3D12Resource*						m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_vertexBufferView;

	ID3D12Resource*						m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW				m_indexBufferView;

	ID3D12Resource*						m_pDepthStencilBuffer;
	ID3D12DescriptorHeap*				m_pDepthStencilDescriptorHeap;

	ConstantBufferWVP					m_cbWVP;

	DirectX::XMFLOAT4X4					m_xmRotationMatrix;
	DirectX::XMFLOAT4X4					m_xmTranslationMatrix;
	DirectX::XMFLOAT4X4					m_xmWorldMatrix;
	
	ID3D12Resource*						m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*								m_pWVPGPUAdress[g_cFrameBufferCount];

	ID3D12RootSignature*				m_pRootSignature;


	int m_iNumCubeIndices;

};

