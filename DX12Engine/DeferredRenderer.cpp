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
	CleanUp();
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

		//Set up lightpass bundles
		m_ppCALightPass[i] = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ppCLLightPass[i] = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, m_ppCALightPass[i]);
		m_ppCLLightPass[i]->Close();
		
		//m_ppCLLightPassBundles[i]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		//m_pRTVhandle[i] = handleDHBackBufferRTVs;
		handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV);
		/*D3DClass::GetDevice()->CreateRenderTargetView(m_ppDiffuseRT[i], nullptr, handleDHDeferredBufferRTVs);
		handleDHDeferredBufferRTVs.Offset(iDHIncrementSizeRTV);
		D3DClass::GetDevice()->CreateRenderTargetView(m_ppNormalAndDepthRT[i], nullptr, handleDHDeferredBufferRTVs);
		handleDHDeferredBufferRTVs.Offset(iDHIncrementSizeRTV);*/
		
		
		m_ppFenceBackBuffer[i] = D3DClass::CreateFence(0, D3D12_FENCE_FLAG_NONE);

	}

	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	//rtvHandle.Offset(D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * D3DClass::GetFrameIndex());
	

	return true;
}

void DeferredRenderer::CleanUp()
{
	SAFE_RELEASE(m_pDHBackBufferRTVs);
	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_ppBackBufferRTV[i]);
		SAFE_RELEASE(m_ppCALightPass[i]);
		SAFE_RELEASE(m_ppCLLightPass[i]);
		SAFE_RELEASE(m_ppFenceBackBuffer[i]);
	}
	return;
}

void DeferredRenderer::RenderLightPass()
{
	int i = D3DClass::GetFrameIndex();
	m_ppCALightPass[i]->Reset();
	m_ppCLLightPass[i]->Reset(m_ppCALightPass[i], nullptr);
	
	m_ppCLLightPass[i]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV * D3DClass::GetFrameIndex());

	m_ppCLLightPass[i]->OMSetRenderTargets(1, &handleDHBackBufferRTVs, NULL, nullptr);
	m_ppCLLightPass[i]->ClearRenderTargetView(handleDHBackBufferRTVs, m_fClearColor, 0, nullptr);

	m_ppCLLightPass[i]->Close();
	D3DClass::QueueGraphicsCommandList(m_ppCLLightPass[i]);
}

void DeferredRenderer::temp_closelistNqueue(ID3D12GraphicsCommandList * pCL)
{
	int i = D3DClass::GetFrameIndex();
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//pCL->Close();
	//D3DClass::QueueGraphicsCommandList(pCL);
}

void DeferredRenderer::temp_setRendertarget(ID3D12GraphicsCommandList * pCL)
{

	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV * D3DClass::GetFrameIndex());
	pCL->OMSetRenderTargets(1, &handleDHBackBufferRTVs, NULL, nullptr);
	
	return;
}


