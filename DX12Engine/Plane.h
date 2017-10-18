#pragma once
#include "Common.h"
#include "D3dClass.h"
#include <DirectXMath.h>
#include "Texture.h"
#include "GPUbridge.h"

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


	//Union used for shader constants
	struct DWParam
	{
		DWParam(FLOAT f) : Float(f) {}
		DWParam(UINT u) : Uint(u) {}

		void operator= (FLOAT f) { Float = f; }
		void operator= (UINT u) { Uint = u; }

		union
		{
			FLOAT Float;
			UINT Uint;
		};
	};
public:
	Plane(GPUbridge* pGPUbridge, IDXGISwapChain3* temp_swapchain);
	~Plane();

	

	void Update(Camera* camera, int iBackBufferIndex);
	void Draw(ID3D12GraphicsCommandList* pCL, Camera* camera, int iBackBufferIndex);


private:
	ID3D12RootSignature*					m_pRootSignature;

	ID3D12PipelineState*					m_pPSO;

	ID3D12Resource*							m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW				m_vertexBufferView;

	ID3D12Resource*							m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW					m_indexBufferView;
	UINT									m_uiNrOfIndices;

	WVPMatrixBufferStruct					m_wvpMat;
	ID3D12Resource*							m_pWVPMatUpploadHeaps[g_iBackBufferCount];
	UINT8*									m_pWVPGPUAdress[g_iBackBufferCount];

	ID3D12Resource*							m_pHeightBuffer;
	ID3D12Resource*							m_pTextureBuffer;
	ID3D12DescriptorHeap*					m_pTextureDH;
	UINT									m_uiTextureDescriptorSize;

};

