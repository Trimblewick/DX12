#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>

#include "PSOHandler.h"
#include "TriangleObject.h"
#include "WindowClass.h"
#include "Camera.h"
#include "Common.h"

#ifdef _DEBUG
#define BUILD_ENABLE_D3D12_DEBUG
#endif

const int g_cFrameBufferCount = 3;//eg double buffering, tripple buffering

class TriangleObject;

class D3DClass
{
private:
	struct Vertex {
		DirectX::XMFLOAT3 pos;
	};


public:
	D3DClass();
	~D3DClass();
	static bool Initialize(const unsigned int cFrameBufferCount, PSOHandler* pPsoHandler);
	static bool Render(Camera* camera, TriangleObject* tri);
	static void Cleanup(); // release com ojects and clean up memory
	static void WaitForPreviousFrame();


	static ID3D12Device* GetDevice();
	static ID3D12CommandAllocator* GetCurrentCommandAllocator();
	static ID3D12Resource* GetCurrentRenderTarget();
	static IDXGISwapChain3* GetSwapChain();
	static ID3D12CommandQueue* GetCommandQueue();

	static void incrementFenceValue();
	static ID3D12Fence* GetCurrentFence();
	static UINT64 GetCurrentFenceValue();

private:
	static ID3D12Device*				m_pDevice;
	static IDXGISwapChain3*				m_pSwapChain;
	static ID3D12CommandQueue*			m_pCommandQueue;


	static ID3D12DescriptorHeap*		m_pRTVDescriptorHeap;
	static ID3D12Resource*				m_pRenderTargets[g_cFrameBufferCount];//right now just for backbuffering
	static ID3D12CommandAllocator*		m_pCommandAllocator[g_cFrameBufferCount];
	static ID3D12Fence*					m_pFence[g_cFrameBufferCount];
	static HANDLE						m_hFenceEventHandle;
	static UINT64						m_ui64FenceValue[g_cFrameBufferCount];
	static unsigned int					m_uiFrameIndex;
	static int							m_iRTVDescriptorSize;

	//temp
	/*static ID3D12GraphicsCommandList*	commandList; // a command list we can record commands into, then execute them to render the frame

	static ID3D12PipelineState*			pipelineStateObject; // pso containing a pipeline state

	static ID3D12RootSignature*			rootSignature; // root signature defines data shaders will access
	static ID3D12Resource*				vertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into

	static D3D12_VERTEX_BUFFER_VIEW		vertexBufferView; // a structure containing a pointer to the vertex data in gpu memory
											   // the total size of the buffer, and the size of each element (vertex)
	*/
};

