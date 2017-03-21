#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>


const int g_cFrameBufferCount = 3;//eg double buffering, tripple buffering

class D3DClass
{
public:
	D3DClass();
	~D3DClass();
	bool Initialize();
	void Render();
	void Cleanup(); // release com ojects and clean up memory
	void SyncLastFence();

private:
	ID3D12Device* m_pDevice;
	IDXGISwapChain3* m_pSwapChain;
	ID3D12CommandQueue* m_pCommandQueue;


	ID3D12DescriptorHeap* m_pRTVDescriptorHeap;
	ID3D12Resource* m_pRenderTargets[g_cFrameBufferCount];//right now just for backbuffering
	ID3D12CommandAllocator* m_pCmmandAllocator[g_cFrameBufferCount];
	ID3D12Fence* m_pFence[g_cFrameBufferCount];
	HANDLE m_hFenceEventHandle;
	UINT64 m_ui64FenceValue[g_cFrameBufferCount];
	unsigned int m_uiFrameIndex;
	int m_iRTVDescriptorSize;


};

