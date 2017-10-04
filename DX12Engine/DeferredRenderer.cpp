#include "DeferredRenderer.h"



DeferredRenderer::DeferredRenderer()
{
	if (!Initialize())
	{
		int stopper = 0;
	}
	
}

DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::Initialize()
{
	m_pDHBackBufferRTVs = D3DClass::CreateDH(g_iBackBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//m_pDHDeferredBufferRTVs = D3DClass::CreateDH(g_iBackBufferCount * 2, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);//right now diffuse and (normals, with depth)

	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	//CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHDeferredBufferRTVs(m_pDHDeferredBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	

	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		D3DClass::GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_ppBackBufferRTV[i]));
		D3DClass::GetDevice()->CreateRenderTargetView(m_ppBackBufferRTV[i], nullptr, handleDHBackBufferRTVs);
		handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV);
		/*D3DClass::GetDevice()->CreateRenderTargetView(m_ppDiffuseRT[i], nullptr, handleDHDeferredBufferRTVs);
		handleDHDeferredBufferRTVs.Offset(iDHIncrementSizeRTV);
		D3DClass::GetDevice()->CreateRenderTargetView(m_ppNormalAndDepthRT[i], nullptr, handleDHDeferredBufferRTVs);
		handleDHDeferredBufferRTVs.Offset(iDHIncrementSizeRTV);*/
		
		m_ppCABackBuffer[i] = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ppFenceBackBuffer[i] = D3DClass::CreateFence(0, D3D12_FENCE_FLAG_NONE);

	}




	return true;
}

void DeferredRenderer::CleanUp()
{
	return;
}

void DeferredRenderer::temp_TransitionCurrentBackBufferRTVIntoRenderState(ID3D12GraphicsCommandList * pCL)
{
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	pCL->OMSetRenderTargets(1, &rtvHandle, NULL, nullptr);

	return;
}

void DeferredRenderer::temp_TransitionCurrentBackBufferRTVIntoPrecentState(ID3D12GraphicsCommandList* pCL)
{
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	
	return;
}

ID3D12GraphicsCommandList * DeferredRenderer::temp_GetGraphicsCommandList_thisFrame()
{
	DxAssert(m_ppCABackBuffer[D3DClass::GetFrameIndex()]->Reset(), S_OK);
	ID3D12GraphicsCommandList* pCL = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, m_ppCABackBuffer[D3DClass::GetFrameIndex()]);
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CD3DX12_CPU_DESCRIPTOR_HANDLE x = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	x.Offset(D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * D3DClass::GetFrameIndex());
	pCL->ClearRenderTargetView(x, g_fClearColor, 0, nullptr);

	return pCL;
}


