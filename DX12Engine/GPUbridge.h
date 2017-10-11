#pragma once
#include "D3dClass.h"

class GPUbridge
{
	
public:
	GPUbridge();
	~GPUbridge();
	void CleanUp();

	static const int							s_iPoolSize = 5;
	
	ID3D12GraphicsCommandList*					GetFreshCL();//return an unused CL from the pool
	void										QueueGraphicsCL(ID3D12GraphicsCommandList* pCL);
	void										ExecuteGrapichsCLs();
	void										WaitForPreviousFrame();
	
private:
	bool										m_bppCADirectPoolFreeFromGPU[g_iBackBufferCount][s_iPoolSize];//occupied by GPU == false, free == true
	ID3D12CommandAllocator*						m_pppCADirectPool[g_iBackBufferCount][s_iPoolSize];

	ID3D12CommandAllocator*						m_ppCLsAssociatedWithCAsInCAPool[g_iBackBufferCount * s_iPoolSize];//cl recording to a cl, or not
	ID3D12GraphicsCommandList*					m_ppCLGraphicsDirectPool[g_iBackBufferCount * s_iPoolSize];

	std::vector<ID3D12CommandList*>				_pCLqueue;

	ID3D12CommandQueue*							m_pCQDirect;
	ID3D12Fence*								m_ppFenceDirect[g_iBackBufferCount];
	UINT64										m_ipFenceValueDirect[g_iBackBufferCount];
	HANDLE										m_fenceEventDirectHandle;
};
