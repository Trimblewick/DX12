#include "DeferredRenderer.h"



DeferredRenderer::DeferredRenderer()
{
	
}

DeferredRenderer::~DeferredRenderer()
{
	CleanUp();
}



bool DeferredRenderer::Initialize(ID3D12CommandQueue* pCQ)
{
	m_pDHBackBufferRTVs = D3DClass::CreateDH(g_iBackBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//m_pDHDeferredBufferRTVs = D3DClass::CreateDH(g_iBackBufferCount * 2, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);//right now diffuse and (normals, with depth)

	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	//CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHDeferredBufferRTVs(m_pDHDeferredBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	
	/*
	////CREATE SWAP CHAIN
	*/

	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = WindowClass::GetWidth(); // buffer width
	backBufferDesc.Height = WindowClass::GetHeight(); // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = g_iBackBufferCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = WindowClass::GetWindowHandler(); // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = !WindowClass::IsFullscreen(); // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	m_pSwapChain = D3DClass::CreateSwapChain(&swapChainDesc, pCQ);
	D3DClass::temp_setsw(m_pSwapChain);
	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppBackBufferRTV[i]));
		D3DClass::GetDevice()->CreateRenderTargetView(m_ppBackBufferRTV[i], nullptr, handleDHBackBufferRTVs);
		

		
		
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
		
		SAFE_RELEASE(m_ppFenceBackBuffer[i]);
	}
	return;
}

void DeferredRenderer::RenderLightPass(ID3D12GraphicsCommandList* pCL)
{
	int iIndex = D3DClass::GetFrameIndex();
	
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV * D3DClass::GetFrameIndex());

	pCL->OMSetRenderTargets(1, &handleDHBackBufferRTVs, NULL, nullptr);
	pCL->ClearRenderTargetView(handleDHBackBufferRTVs, m_fClearColor, 0, nullptr);

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

void DeferredRenderer::temp_swap()
{
	m_pSwapChain->Present(0, 0);
}


