#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>

#include "WindowClass.h"


const int g_cFrameBufferCount = 3;//eg double buffering, tripple buffering

class D3DClass
{
private:
	struct Vertex {
		DirectX::XMFLOAT3 pos;
	};


public:
	D3DClass();
	~D3DClass();
	bool Initialize(const WindowClass window, const unsigned int cFrameBufferCount);
	void Render();
	void Cleanup(); // release com ojects and clean up memory
	void SyncLastFence();

private:
	ID3D12Device* m_pDevice;
	IDXGISwapChain3* m_pSwapChain;
	ID3D12CommandQueue* m_pCommandQueue;


	ID3D12DescriptorHeap* m_pRTVDescriptorHeap;
	ID3D12Resource* m_pRenderTargets[g_cFrameBufferCount];//right now just for backbuffering
	ID3D12CommandAllocator* m_pCommandAllocator[g_cFrameBufferCount];
	ID3D12Fence* m_pFence[g_cFrameBufferCount];
	HANDLE m_hFenceEventHandle;
	UINT64 m_ui64FenceValue[g_cFrameBufferCount];
	unsigned int m_uiFrameIndex;
	int m_iRTVDescriptorSize;


	//temp
	ID3D12GraphicsCommandList* commandList; // a command list we can record commands into, then execute them to render the frame

	ID3D12PipelineState* pipelineStateObject; // pso containing a pipeline state

	ID3D12RootSignature* rootSignature; // root signature defines data shaders will access

	D3D12_VIEWPORT viewport; // area that output from rasterizer will be stretched to.

	D3D12_RECT scissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	ID3D12Resource* vertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView; // a structure containing a pointer to the vertex data in gpu memory
											   // the total size of the buffer, and the size of each element (vertex)
	
};

