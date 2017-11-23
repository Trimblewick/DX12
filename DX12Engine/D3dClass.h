#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>

#include "WindowClass.h"
#include "Common.h"
#include "Shader.h"

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
	static ID3D12Fence*							CreateFence();
	static ID3D12DescriptorHeap*				CreateDH(int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
	static ID3D12GraphicsCommandList*			CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE listType, ID3D12CommandAllocator* pCA);
	static ID3D12CommandQueue*					CreateCQ(D3D12_COMMAND_LIST_TYPE listType);
	static IDXGISwapChain3*						CreateSwapChain(DXGI_SWAP_CHAIN_DESC* pDesc, ID3D12CommandQueue* pCQ);
	static ID3D12Resource*						CreateCommittedResource(D3D12_HEAP_TYPE heapType, UINT iBufferSize, D3D12_RESOURCE_STATES resourceState, LPCWSTR bufferName);
	static ID3D12RootSignature*					CreateRS(D3D12_ROOT_SIGNATURE_DESC* pDesc);
	static ID3D12RootSignature*					CreateRS(Shader* pShader);
	static ID3D12PipelineState*					CreateGraphicsPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc);
	static ID3D12PipelineState*					CreateGraphicsPSO(Shader* pShader, ID3D12RootSignature* pRS);

	static ID3D12Device*						GetDevice();
	

private:
	static IDXGIFactory4*				s_pDXGIFactory;
	static ID3D12Device*				s_pDevice;

	
};

