#pragma once
#include "D3dClass.h"
#include "Common.h"


class DeferredRenderer
{
private:
	

public:
	DeferredRenderer();
	~DeferredRenderer();
	bool Initialize();
	void CleanUp();

	void RenderLightPass(ID3D12GraphicsCommandList* pCL);
	void temp_closelistNqueue(ID3D12GraphicsCommandList * pCL);

	void temp_setRendertarget(ID3D12GraphicsCommandList* pCL);
	

private:
	//ID3D12CommandAllocator*							m_ppCADeferredBuffer[g_iBackBufferCount];//Used when recording geometrypass
	//ID3D12CommandAllocator*							m_ppCALightPass[g_iBackBufferCount];//Used when recording for lightpass
	//ID3D12GraphicsCommandList*						m_ppCLLightPass[g_iBackBufferCount];

	ID3D12DescriptorHeap*							m_pDHDeferredBufferRTVs;
	ID3D12DescriptorHeap*							m_pDHBackBufferRTVs;
	//int											m_iDHIncrementSizeRTV;
	//CD3DX12_CPU_DESCRIPTOR_HANDLE					m_DHHandleBackBufferRTVs;

	ID3D12Resource*									m_ppDiffuseRT[g_iBackBufferCount];
	ID3D12Resource*									m_ppNormalAndDepthRT[g_iBackBufferCount];
	//backbuffer
	ID3D12Resource*									m_ppBackBufferRTV[g_iBackBufferCount];
	//CD3DX12_CPU_DESCRIPTOR_HANDLE					m_pRTVhandle[g_iBackBufferCount];

	ID3D12Fence*									m_ppFenceBackBuffer[g_iBackBufferCount];
	HANDLE											m_handleFenceEvent;
	unsigned int									m_iFrameIndex;
	

	const float										m_fClearColor[4] = { 1.0f, 0.0f, 0.5f, 1.0f };
};

