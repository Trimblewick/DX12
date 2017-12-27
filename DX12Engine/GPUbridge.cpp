#include "GPUbridge.h"


GPUbridge::GPUbridge()
{
	_iBackBufferIndex = 0;
	m_pCQDirect = D3DClass::CreateCQ(D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_pCQCompute = D3DClass::CreateCQ(D3D12_COMMAND_LIST_TYPE_COMPUTE);
	
	for (int k = 0; k < g_iBackBufferCount; ++k)
	{
		for (int i = 0; i < s_iPoolSize; ++i)
		{
			m_bppCADirectPoolFreeFromGPU[k][i] = true;
			m_pppCADirectPool[k][i] = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
		}
		m_ppFenceDirect[k] = D3DClass::CreateFence();
		m_ipFenceValueDirect[k] = 0;
	}
	m_fenceEventDirectHandle = CreateEvent(nullptr, NULL, NULL, nullptr);
	assert(m_fenceEventDirectHandle != nullptr);

	for (int i = 0; i < s_iPoolSize; ++i)
	{
		m_ppCLGraphicsDirectPool[i] = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, m_pppCADirectPool[0][0]);
		m_ppCLGraphicsDirectPool[i]->Close();
		m_ppCLsAssociatedWithCAsInCAPool[i] = nullptr;
	}
}

GPUbridge::~GPUbridge()
{
	CleanUp();
}

void GPUbridge::CleanUp()
{
	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		m_ipFenceValueDirect[i]++;											//Increment each fencevalue to be above the completed value in the fence.
		m_pCQDirect->Signal(m_ppFenceDirect[i], m_ipFenceValueDirect[i]);	//Signal the CQ again for each frame and wait to make sure the GPU is finished with it.

		DxAssert(m_ppFenceDirect[i]->SetEventOnCompletion(m_ipFenceValueDirect[i], m_fenceEventDirectHandle), S_OK);
		WaitForSingleObject(m_fenceEventDirectHandle, INFINITE);
	}

	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		for (int j = 0; j < s_iPoolSize; ++j)
		{
			SAFE_RELEASE(m_pppCADirectPool[i][j]);
		}
	}
	for (int i = 0; i < s_iPoolSize; ++i)
	{

		SAFE_RELEASE(m_ppCLGraphicsDirectPool[i]);
	}

	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_ppFenceDirect[i]);
	}

	SAFE_RELEASE(m_pCQDirect);
}

ID3D12CommandQueue * GPUbridge::GetCQ()
{
	return m_pCQDirect;
}

ID3D12GraphicsCommandList* GPUbridge::GetFreshCL()
{
	int iFirstUnoccupiedCL = -1;
	ID3D12GraphicsCommandList* pCL = nullptr;
	bool bCAoccupied = false;
	for (int i = 0; i < s_iPoolSize && iFirstUnoccupiedCL == -1; ++i)
	{
		if (m_bppCADirectPoolFreeFromGPU[_iBackBufferIndex][i])//is CA unoccupied by gpu?
		{
			
			for (int k = 0; k < s_iPoolSize && iFirstUnoccupiedCL == -1; ++k)//is CA occupied by another CL? 
			{
				
				if (m_ppCLsAssociatedWithCAsInCAPool[k] == nullptr)// m_pppCADirectPool[iIndex][i])
				{
					iFirstUnoccupiedCL = k;
				}
				
			}
			if (iFirstUnoccupiedCL > -1)//if CA is unoccupied
			{
				DxAssert(m_ppCLGraphicsDirectPool[iFirstUnoccupiedCL]->Reset(m_pppCADirectPool[_iBackBufferIndex][i], nullptr), S_OK);
				pCL = m_ppCLGraphicsDirectPool[iFirstUnoccupiedCL];
				m_ppCLsAssociatedWithCAsInCAPool[iFirstUnoccupiedCL] = m_pppCADirectPool[_iBackBufferIndex][i];
				m_bppCADirectPoolFreeFromGPU[_iBackBufferIndex][i] = false;
			}
			
		}
	}
	if (iFirstUnoccupiedCL == -1)
	{
		assert(false);//No CLs availible. Have you made sure they get queued?
		return nullptr;
	}
	return pCL;
}

void GPUbridge::QueueGraphicsCL(ID3D12GraphicsCommandList* pCL)
{
	for (int i = 0; i < s_iPoolSize; ++i)
	{
		if (pCL == m_ppCLGraphicsDirectPool[i])
		{
			m_ppCLsAssociatedWithCAsInCAPool[i] = nullptr;//Enable CL to cpu
			break;
		}
	}
	pCL->Close();

	_pCLqueue.push_back(pCL);
}

void GPUbridge::ExecuteGrapichsCLs()
{
	m_pCQDirect->ExecuteCommandLists(_pCLqueue.size(), _pCLqueue.data());

	m_pCQDirect->Signal(m_ppFenceDirect[_iBackBufferIndex], m_ipFenceValueDirect[_iBackBufferIndex]);
	_pCLqueue.clear();
}

void GPUbridge::ExecuteDecoupledCLs(int iNOCLs, ID3D12CommandList ** ppCLs, _In_opt_ ID3D12Fence* pFence, _In_opt_ int iFenceValue)
{
	m_pCQDirect->ExecuteCommandLists(iNOCLs, ppCLs);

	if (pFence)
	{
		m_pCQDirect->Signal(pFence, iFenceValue);
	}
}

void GPUbridge::WaitForPreviousFrame(int iBackBufferIndex)
{
	_iBackBufferIndex = iBackBufferIndex;

	if (m_ppFenceDirect[iBackBufferIndex]->GetCompletedValue() < m_ipFenceValueDirect[iBackBufferIndex])
	{
		DxAssert(m_ppFenceDirect[iBackBufferIndex]->SetEventOnCompletion(m_ipFenceValueDirect[iBackBufferIndex], m_fenceEventDirectHandle), S_OK);

		WaitForSingleObject(m_fenceEventDirectHandle, INFINITE);

	}

	//reset used CAs
	for (int i = 0; i < s_iPoolSize; ++i)
	{
		if (!m_bppCADirectPoolFreeFromGPU[iBackBufferIndex][i] && m_ppCLsAssociatedWithCAsInCAPool[i] == nullptr)//if the cl is queued, and now has finished it's executeion. Reset CA.
		{
			DxAssert(m_pppCADirectPool[iBackBufferIndex][i]->Reset(), S_OK);
			m_bppCADirectPoolFreeFromGPU[iBackBufferIndex][i] = true;
		}
	}

	m_ipFenceValueDirect[iBackBufferIndex]++;//Frame has transitioned, increment value

}

void GPUbridge::ExecuteDecoupledComputeCL(ID3D12CommandList * pCL, ID3D12Fence * pFenceHandle, int iFenceValue)
{
	ID3D12CommandList* ppCLs[] = { pCL };
	m_pCQCompute->ExecuteCommandLists(1, ppCLs);

	if (pFenceHandle)
	{
		m_pCQCompute->Signal(pFenceHandle, iFenceValue);
	}
}

