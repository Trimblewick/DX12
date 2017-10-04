#pragma once
#include "D3dClass.h"
#include "Common.h"

const int g_iBackBufferCount = 3;
const float g_fClearColor[4] = { 1.0f, 0.0f, 0.5f, 1.0f };

class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();
	bool Initialize();
	void CleanUp();

	void temp_TransitionCurrentBackBufferRTVIntoRenderState(ID3D12GraphicsCommandList* pCL);
	void temp_TransitionCurrentBackBufferRTVIntoPrecentState(ID3D12GraphicsCommandList* pCL);

	ID3D12GraphicsCommandList*						temp_GetGraphicsCommandList_thisFrame();//temp, bound to currentframes commandallocator
	

private:
	//*****
	//Note that only one command list associated with each command allocator may be in a recording state at one time
	//****** shit
	ID3D12CommandAllocator*							m_ppCADeferredBuffer[g_iBackBufferCount];//Used when recording geometrypass
	ID3D12CommandAllocator*							m_ppCABackBuffer[g_iBackBufferCount];//Used when recording for lightpass

	ID3D12DescriptorHeap*							m_pDHDeferredBufferRTVs;
	ID3D12DescriptorHeap*							m_pDHBackBufferRTVs;
	//int											m_iDHIncrementSizeRTV;
	//CD3DX12_CPU_DESCRIPTOR_HANDLE					m_DHHandleBackBufferRTVs;

	ID3D12Resource*									m_ppDiffuseRT[g_iBackBufferCount];
	ID3D12Resource*									m_ppNormalAndDepthRT[g_iBackBufferCount];
	//backbuffer
	ID3D12Resource*									m_ppBackBufferRTV[g_iBackBufferCount];

	ID3D12Fence*									m_ppFenceBackBuffer[g_iBackBufferCount];
	HANDLE											m_handleFenceEvent;
	unsigned int									m_iFrameIndex;
	
};

