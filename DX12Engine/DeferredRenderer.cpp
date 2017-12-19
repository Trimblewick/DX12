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
	m_pDHBackBufferRTVs = D3DClass::CreateDH(g_iBackBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
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
	
	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		HRESULT hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppBackBufferRTV[i]));// reinterpret_cast<void**>(&m_ppBackBufferRTV[i]));
		//m_ppBackBufferRTV[i]
		//D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, iDHIncrementSizeRTV, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"Backbuffer");
		D3DClass::GetDevice()->CreateRenderTargetView(m_ppBackBufferRTV[i], nullptr, handleDHBackBufferRTVs);
		//CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[i], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		//D3DClass::GetDevice()->CreateUnorderedAccessView(m_ppBackBufferRTV[i], nullptr, nullptr, handleDHBackBufferRTVs);
		


		//m_ppCLLightPassBundles[i]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[D3DClass::GetFrameIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		//m_pRTVhandle[i] = handleDHBackBufferRTVs;
		handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV);
		/*D3DClass::GetDevice()->CreateRenderTargetView(m_ppDiffuseRT[i], nullptr, handleDHDeferredBufferRTVs);
		handleDHDeferredBufferRTVs.Offset(iDHIncrementSizeRTV);
		D3DClass::GetDevice()->CreateRenderTargetView(m_ppNormalAndDepthRT[i], nullptr, handleDHDeferredBufferRTVs);
		handleDHDeferredBufferRTVs.Offset(iDHIncrementSizeRTV);*/
		
		//m_ppFenceBackBuffer[i] = D3DClass::CreateFence();

	}

	

	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	//rtvHandle.Offset(D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * D3DClass::GetFrameIndex());
	

	return true;
}

void DeferredRenderer::CleanUp()
{
	/*BOOL fs = false;
	if (m_pSwapChain->GetFullscreenState(&fs, NULL))
		m_pSwapChain->SetFullscreenState(false, NULL);
		*/

	
	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_ppBackBufferRTV[i]);
		
		SAFE_RELEASE(m_ppFenceBackBuffer[i]);
	}
	SAFE_RELEASE(m_pDHBackBufferRTVs);
	SAFE_RELEASE(m_pSwapChain);
	return;
}

IDXGISwapChain3 * DeferredRenderer::GetSwapChain()
{
	return m_pSwapChain;
}

int DeferredRenderer::GetBackBufferIndex()
{
	return m_pSwapChain->GetCurrentBackBufferIndex();
}

void DeferredRenderer::DrawObjects(Object ** ppObjects, int iNrOfObjects, ID3D12GraphicsCommandList* pCL)
{
	for (int i = 0; i < iNrOfObjects; ++i)
	{
		pCL->IASetPrimitiveTopology(ppObjects[i]->GetMesh()->GetPrimitiveTopology());
		pCL->IASetVertexBuffers(0, 1, &ppObjects[i]->GetMesh()->GetVertexBufferView());
		pCL->DrawInstanced(ppObjects[i]->GetMesh()->GetNrOfVertices(), 1, 0, 0);
	}
}

void DeferredRenderer::UnlockNextRTV(ID3D12CommandList* pCL)
{
	int iBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	if (m_ppFenceBackBuffer[iBackBufferIndex]->GetCompletedValue() < m_pFenceValuesBackBuffer[iBackBufferIndex])
	{
		DxAssert(m_ppFenceBackBuffer[iBackBufferIndex]->SetEventOnCompletion(m_pFenceValuesBackBuffer[iBackBufferIndex], m_handleFenceEvent), S_OK);

		WaitForSingleObject(m_handleFenceEvent, INFINITE);
	}
	m_pFenceValuesBackBuffer[iBackBufferIndex]++;


}

void DeferredRenderer::PresentCurrentRTV()
{
}

D3D12_GPU_DESCRIPTOR_HANDLE DeferredRenderer::GetRTVDHhandle()
{
	return m_pDHBackBufferRTVs->GetGPUDescriptorHandleForHeapStart();
}

ID3D12DescriptorHeap* DeferredRenderer::GetDH()
{
	return m_pDHBackBufferRTVs;
}


void DeferredRenderer::RenderLightPass(ID3D12GraphicsCommandList* pCL)
{
	int iIndex = this->GetBackBufferIndex();
	
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[iIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	

	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV * iIndex);

	pCL->OMSetRenderTargets(1, &handleDHBackBufferRTVs, NULL, nullptr);
	pCL->ClearRenderTargetView(handleDHBackBufferRTVs, m_fClearColor, 0, nullptr);

}

void DeferredRenderer::temp_closelistNqueue(ID3D12GraphicsCommandList * pCL)
{
	int iIndex = this->GetBackBufferIndex();
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ppBackBufferRTV[iIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

}

void DeferredRenderer::temp_setRendertarget(ID3D12GraphicsCommandList * pCL)
{
	int iIndex = this->GetBackBufferIndex();
	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handleDHBackBufferRTVs(m_pDHBackBufferRTVs->GetCPUDescriptorHandleForHeapStart());
	handleDHBackBufferRTVs.Offset(iDHIncrementSizeRTV * iIndex);
	pCL->OMSetRenderTargets(1, &handleDHBackBufferRTVs, NULL, nullptr);
	
	return;
}

void DeferredRenderer::PrecentNextFrame()
{
	DxAssert(m_pSwapChain->Present(0, 0), S_OK);
}


