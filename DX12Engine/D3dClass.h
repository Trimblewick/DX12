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
	static bool Initialize();
	
	static void Cleanup(); // release com ojects and clean up memory
	static void WaitForPreviousFrame();


	static ID3D12Device* GetDevice();
	static ID3D12CommandAllocator* GetCurrentCommandAllocator();
	static ID3D12Resource* GetCurrentRenderTarget();
	static IDXGISwapChain3* GetSwapChain();
	static ID3D12CommandQueue* GetCommandQueue();
	
	static unsigned int GetFrameIndex();

	static D3D12_CPU_DESCRIPTOR_HANDLE* GetRTVDescriptorHandle();

	static void IncrementFenceValue();
	static ID3D12Fence* GetCurrentFence();
	static UINT64 GetCurrentFenceValue();

	//static ID3D12GraphicsCommandList* GetNewOffGraphicsCommandList(_In_opt_ ID3D12PipelineState* pso = nullptr);

	static void QueueGraphicsCommandList(ID3D12CommandList* pCL);
	static void ExecuteGraphicsCommandLists();


private:
	static ID3D12Device*				s_pDevice;
	static IDXGISwapChain3*				s_pSwapChain;
	static ID3D12CommandQueue*			s_pCommandQueue;

	static D3D12_CPU_DESCRIPTOR_HANDLE* s_pRTVHandle;//pointer to allocated memory for the GetRTVDescriptorHande() function.

	static std::vector<ID3D12CommandList*> _pGraphicsCommandLists;

	static ID3D12DescriptorHeap*		s_pRTVDescriptorHeap;
	static ID3D12Resource*				s_pRenderTargets[g_cFrameBufferCount];//right now just for backbuffering
	static ID3D12CommandAllocator*		s_pCommandAllocator[g_cFrameBufferCount];
	static ID3D12Fence*					s_pFence[g_cFrameBufferCount];
	static HANDLE						s_hFenceEventHandle;
	static UINT64						s_ui64FenceValue[g_cFrameBufferCount];
	static unsigned int					s_uiFrameIndex;
	static int							s_iRTVDescriptorSize;
};

