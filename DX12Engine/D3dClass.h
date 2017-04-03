#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>

#include "PSOHandler.h"
#include "WindowClass.h"
#include "Camera.h"
#include "Common.h"

#ifdef _DEBUG
#define BUILD_ENABLE_D3D12_DEBUG
#endif

const int g_cFrameBufferCount = 3;//eg double buffering, tripple buffering


class D3DClass
{
private:
	

public:
	D3DClass();
	~D3DClass();
	static bool Initialize(const unsigned int cFrameBufferCount, PSOHandler* pPsoHandler);
	
	static void Cleanup(); // release com ojects and clean up memory
	static void WaitForPreviousFrame();


	static ID3D12Device* GetDevice();
	static ID3D12CommandAllocator* GetCurrentCommandAllocator();
	static ID3D12Resource* GetCurrentRenderTarget();
	static IDXGISwapChain3* GetSwapChain();
	static ID3D12CommandQueue* GetCommandQueue();
	
	static unsigned int GetFrameIndex();
	static int GetRTVDescriptorSize();
	static ID3D12DescriptorHeap* GetRTVDescriptorHeap();

	

	static void IncrementFenceValue();
	static ID3D12Fence* GetCurrentFence();
	static UINT64 GetCurrentFenceValue();


	static void QueueGraphicsCommandList(ID3D12CommandList* pCL);
	static void ExecuteGraphicsCommandLists();


private:
	static ID3D12Device*				m_pDevice;
	static IDXGISwapChain3*				m_pSwapChain;
	static ID3D12CommandQueue*			m_pCommandQueue;


	//static ID3D12CommandList*			m_pClearColorBundle;

	static std::vector<ID3D12CommandList*> _vGraphicsCommandLists;

	static ID3D12DescriptorHeap*		m_pRTVDescriptorHeap;
	static ID3D12Resource*				m_pRenderTargets[g_cFrameBufferCount];//right now just for backbuffering
	static ID3D12CommandAllocator*		m_pCommandAllocator[g_cFrameBufferCount];
	static ID3D12Fence*					m_pFence[g_cFrameBufferCount];
	static HANDLE						m_hFenceEventHandle;
	static UINT64						m_ui64FenceValue[g_cFrameBufferCount];
	static unsigned int					m_uiFrameIndex;
	static int							m_iRTVDescriptorSize;
};

