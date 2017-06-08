#include "FrameBuffer.h"


FrameBuffer::FrameBuffer()
{
	
}

FrameBuffer::~FrameBuffer()
{
}

bool FrameBuffer::Initialize()
{
	ID3D12GraphicsCommandList* pCL;

	D3DClass::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3DClass::GetCurrentCommandAllocator(), NULL, IID_PPV_ARGS(&pCL));

	m_pGraphicsCommandLists.push_back(pCL);

	DxAssert(pCL->Close(), S_OK);

	//Create a descriptor heap for depthstencil
	D3D12_DESCRIPTOR_HEAP_DESC depthStencilStateDescriptorHeadpDesc = {};
	depthStencilStateDescriptorHeadpDesc.NumDescriptors = 1;
	depthStencilStateDescriptorHeadpDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	depthStencilStateDescriptorHeadpDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ID3D12DescriptorHeap* pDH;
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&depthStencilStateDescriptorHeadpDesc, IID_PPV_ARGS(&pDH)), S_OK);

	m_pDepthStencilsDescriptorHeaps.push_back(pDH);

	//create depthstencil
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = depthStencilViewDesc.Format;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;


	ID3D12Resource* pDB;
	D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WindowClass::GetWidth(), WindowClass::GetHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&pDB)
		);
	pDB->SetName(L"Standard Depth/Stencil Resource Heap");

	D3DClass::GetDevice()->CreateDepthStencilView(pDB, &depthStencilViewDesc, pDH->GetCPUDescriptorHandleForHeapStart());
	m_pDepthStencilsBuffers.push_back(pDB);

	return true;
}

void FrameBuffer::CleanUp()
{
	for (ID3D12CommandList* cl : m_pGraphicsCommandLists)
	{
		SAFE_RELEASE(cl);
	}
	for (ID3D12DescriptorHeap* dh : m_pDepthStencilsDescriptorHeaps)
	{
		SAFE_RELEASE(dh);
	}
	for (ID3D12Resource* db : m_pDepthStencilsBuffers)
	{
		SAFE_RELEASE(db);
	}

	m_pDepthStencilsDescriptorHeaps.clear();
	m_pDepthStencilsBuffers.clear();
	m_pGraphicsCommandLists.clear();
}


bool FrameBuffer::ResetList(PIPELINES i)// , D3D12_CPU_DESCRIPTOR_HANDLE * pRTVHandle, ID3D12PipelineState * pPSO)
{
	_pRTVHandle = D3DClass::GetRTVDescriptorHandle();
	//start recording
	DxAssert(m_pGraphicsCommandLists[i]->Reset(D3DClass::GetCurrentCommandAllocator(), nullptr), S_OK);
	m_pGraphicsCommandLists[i]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(D3DClass::GetCurrentRenderTarget(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	//set rendertarget
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptorHandle(m_pDepthStencilsDescriptorHeaps[i]->GetCPUDescriptorHandleForHeapStart());
	m_pGraphicsCommandLists[i]->OMSetRenderTargets(1, _pRTVHandle, FALSE, &depthStencilDescriptorHandle);

	//clear views
	m_pGraphicsCommandLists[i]->ClearDepthStencilView(m_pDepthStencilsDescriptorHeaps[i]->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	m_pGraphicsCommandLists[i]->ClearRenderTargetView(*_pRTVHandle, m_fClearColor, 0, nullptr);

	return true;
}

bool FrameBuffer::CloseList(PIPELINES i)
{
	m_pGraphicsCommandLists[i]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(D3DClass::GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	DxAssert(m_pGraphicsCommandLists[i]->Close(), S_OK);

	D3DClass::QueueGraphicsCommandList(m_pGraphicsCommandLists[i]);
	return true;
}

void FrameBuffer::SetRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE * pRTVHandle)
{
	_pRTVHandle = pRTVHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE * FrameBuffer::GetRTVHandle()
{
	return _pRTVHandle;
}

ID3D12GraphicsCommandList * FrameBuffer::GetGraphicsCommandList(PIPELINES i)
{
	return m_pGraphicsCommandLists[i];
}
