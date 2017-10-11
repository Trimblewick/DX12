#include "GPUbridge.h"


GPUbridge::GPUbridge()
{
	m_pCQDirect = D3DClass::CreateCQ(D3D12_COMMAND_LIST_TYPE_DIRECT);
	
	

	for (int k = 0; k < g_iBackBufferCount; ++k)
	{
		for (int i = 0; i < s_iPoolSize; ++i)
		{
			m_bppCADirectPoolFreeFromGPU[k][i] = true;
			m_pppCADirectPool[k][i] = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
		}
		m_ppFenceDirect[k] = D3DClass::CreateFence(0, D3D12_FENCE_FLAG_NONE);
		m_ipFenceValueDirect[k] = 0;
	}
	m_fenceEventDirectHandle = CreateEvent(nullptr, NULL, NULL, nullptr);
	assert(m_fenceEventDirectHandle != nullptr);

	for (int i = 0; i < g_iBackBufferCount * s_iPoolSize; ++i)
	{
		m_ppCLGraphicsDirectPool[i] = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, m_pppCADirectPool[0][0]);
		m_ppCLGraphicsDirectPool[i]->Close();
		m_ppCLsAssociatedWithCAsInCAPool[i] = nullptr;
	}
}

GPUbridge::~GPUbridge()
{
}

void GPUbridge::CleanUp()
{
}

ID3D12CommandQueue * GPUbridge::GetCQ()
{
	return m_pCQDirect;
}

ID3D12GraphicsCommandList* GPUbridge::GetFreshCL()
{
	int iIndex = D3DClass::GetFrameIndex();
	int iFirstUnoccupiedCL = -1;
	ID3D12GraphicsCommandList* pCL = nullptr;
	bool bCAoccupied = false;
	for (int i = 0; i < s_iPoolSize && iFirstUnoccupiedCL == -1; ++i)
	{
		if (m_bppCADirectPoolFreeFromGPU[iIndex][i])//is CA unoccupied by gpu?
		{
			
			for (int k = 0; k < g_iBackBufferCount * s_iPoolSize && !bCAoccupied; ++k)//is CA occupied by another CL? 
			{
				if (iFirstUnoccupiedCL == -1 && m_ppCLsAssociatedWithCAsInCAPool[k] == nullptr)
				{
					iFirstUnoccupiedCL = k;
				}
				if (m_ppCLsAssociatedWithCAsInCAPool[k] == m_pppCADirectPool[iIndex][i])
				{
					bCAoccupied = true;
				}
				
			}
			if (!bCAoccupied)//if CA is unoccupied
			{
				DxAssert(m_ppCLGraphicsDirectPool[iFirstUnoccupiedCL]->Reset(m_pppCADirectPool[iIndex][i], nullptr), S_OK);
				pCL = m_ppCLGraphicsDirectPool[iFirstUnoccupiedCL];
				m_ppCLsAssociatedWithCAsInCAPool[iFirstUnoccupiedCL] = m_pppCADirectPool[iIndex][i];
			}
			
		}
	}
	if (iFirstUnoccupiedCL == -1)
	{
		assert(false);//<<--- this section is fragile?
		return nullptr;
	}
	return pCL;
}

void GPUbridge::QueueGraphicsCL(ID3D12GraphicsCommandList* pCL)
{
	for (int i = 0; i < g_iBackBufferCount * s_iPoolSize; ++i)
	{
		if (pCL == m_ppCLGraphicsDirectPool[i])
		{
			m_ppCLsAssociatedWithCAsInCAPool[i] = nullptr;//Enable CA to cpu
			break;
		}
	}
	pCL->Close();

	_pCLqueue.push_back(pCL);
}

void GPUbridge::ExecuteGrapichsCLs()
{
	int iIndex = D3DClass::GetFrameIndex();
	m_pCQDirect->ExecuteCommandLists(_pCLqueue.size(), _pCLqueue.data());

	m_pCQDirect->Signal(m_ppFenceDirect[iIndex], m_ipFenceValueDirect[iIndex]);
	_pCLqueue.clear();
}

void GPUbridge::WaitForPreviousFrame()
{
	int iIndex = D3DClass::GetFrameIndex();

	if (m_ppFenceDirect[iIndex]->GetCompletedValue() < m_ipFenceValueDirect[iIndex])
	{
		DxAssert(m_ppFenceDirect[iIndex]->SetEventOnCompletion(m_ipFenceValueDirect[iIndex], m_fenceEventDirectHandle), S_OK);

		WaitForSingleObject(m_fenceEventDirectHandle, INFINITE);

	}
	//reset used CAs
	for (int i = 0; i < s_iPoolSize; ++i)
	{
		if (!m_bppCADirectPoolFreeFromGPU[iIndex][i])
		{
			DxAssert(m_pppCADirectPool[iIndex][i]->Reset(), S_OK);
			m_bppCADirectPoolFreeFromGPU[iIndex][i] = true;
		}
	}

	m_ipFenceValueDirect[iIndex]++;

}

