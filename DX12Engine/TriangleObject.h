#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include "Common.h"
#include "D3dClass.h"
#include "PSOHandler.h"
#include "Camera.h"

class TriangleObject
{
private:
	struct Vertex 
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color; 
	};

	struct CBColorMultiplyer
	{
		DirectX::XMFLOAT4 colorMultiplier;
	};


public:
	TriangleObject(PSOHandler* pPsoHandler);
	~TriangleObject();
	void Draw(D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, Camera* camera);
	void Update();

	ID3D12CommandList* GetCommandList();

private:

	ID3D12PipelineState*			_pPSO;
	ID3D12GraphicsCommandList*		m_pCommandList;
	ID3D12RootSignature*			m_pRootSignature;
	ID3D12Resource*					m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_vertexBufferView;

	ID3D12Resource*					m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW			m_indexBufferView;

	ID3D12Resource*					m_pDepthStencilBuffer;
	ID3D12DescriptorHeap*			m_pDepthStencilDescriptorHeap;

	ID3D12Resource*					m_ppColorMultiplyerConstantBufferUpploadHeap[g_cFrameBufferCount];
	ID3D12DescriptorHeap*			m_ppColorMultiplyerDescriptorHeap[g_cFrameBufferCount];

	UINT8*							m_ui8ColorMultiplyerGPUAddress[g_cFrameBufferCount];

	CBColorMultiplyer				m_colorMultiplyerData;
};
