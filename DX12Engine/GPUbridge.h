#pragma once
#include "D3dClass.h"

class GPUbridge
{
	
public:
	GPUbridge();
	~GPUbridge();

	static const int						m_iPoolSize = 5;

	ID3D12GraphicsCommandList*				GetFreshCL();//return an unused CL from the pool
	void QueueGrapichsCL(ID3D12GraphicsCommandList* pCL);
	void ExecuteGrapichsCQ();

private:
	bool									m_ppbActivePool[g_iBackBufferCount][m_iPoolSize];
	ID3D12CommandAllocator*					m_pppCADirectPool[g_iBackBufferCount][m_iPoolSize];
	ID3D12GraphicsCommandList*				m_pppCLGraphicsDirectPool[g_iBackBufferCount][m_iPoolSize];

	ID3D12CommandQueue*						m_pCQDirect;

};

