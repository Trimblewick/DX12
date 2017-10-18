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

const int g_iBackBufferCount = 3;


class D3DClass
{
private:
	

public:
	D3DClass();
	~D3DClass();
	static bool Initialize();
	
	static void Cleanup(); // release com ojects and clean up memory
	

	static ID3D12CommandAllocator*				CreateCA(D3D12_COMMAND_LIST_TYPE listType);
	static ID3D12Fence*							CreateFence(UINT64 ui64InitVal, D3D12_FENCE_FLAGS fenceFlag);
	static ID3D12DescriptorHeap*				CreateDH(int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
	static ID3D12GraphicsCommandList*			CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE listType, ID3D12CommandAllocator* pCA);
	static ID3D12CommandQueue*					CreateCQ(D3D12_COMMAND_LIST_TYPE listType);
	static IDXGISwapChain3*						CreateSwapChain(DXGI_SWAP_CHAIN_DESC* desc, ID3D12CommandQueue* pCQ);

	static ID3D12Device*						GetDevice();
	

private:
	static IDXGIFactory4*				s_pDXGIFactory;
	static ID3D12Device*				s_pDevice;

	
};

