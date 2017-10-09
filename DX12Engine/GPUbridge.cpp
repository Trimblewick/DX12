#include "GPUbridge.h"



GPUbridge::GPUbridge()
{
	m_pCQDirect = D3DClass::CreateCQ(D3D12_COMMAND_LIST_TYPE_DIRECT);
	for (int k = 0; k < g_iBackBufferCount; ++k)
	{
		for (int i = 0; i < m_iPoolSize; ++i)
		{
			m_ppbActivePool[k][i] = false;
			m_pppCADirectPool[k][i] = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
			m_pppCLGraphicsDirectPool[k][i] = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, m_pppCADirectPool[k][i]);
		}
	}
}

GPUbridge::~GPUbridge()
{
}

ID3D12GraphicsCommandList* GPUbridge::GetFreshCL()
{
	int iIndex = D3DClass::GetFrameIndex();
	bool bFoundFreeCL = false;
	ID3D12GraphicsCommandList* pCL = nullptr;
	for (int i = 0; i < m_iPoolSize && !bFoundFreeCL; ++i)
	{
		if (!m_ppbActivePool[iIndex][i])
		{
			m_ppbActivePool[iIndex][i] = true;
			pCL = m_pppCLGraphicsDirectPool[iIndex][i];

			bFoundFreeCL = true;
		}
	}


	assert(bFoundFreeCL);
	return pCL;
}

void GPUbridge::QueueGrapichsCL(ID3D12GraphicsCommandList * pCL)
{
	
}

void GPUbridge::ExecuteGrapichsCQ()
{
}

