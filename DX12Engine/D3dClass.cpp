
#include "D3dClass.h"



ID3D12Device* D3DClass::m_pDevice;
IDXGISwapChain3* D3DClass::m_pSwapChain;
ID3D12CommandQueue* D3DClass::m_pCommandQueue;


ID3D12DescriptorHeap* D3DClass::m_pRTVDescriptorHeap;
ID3D12Resource* D3DClass::m_pRenderTargets[g_cFrameBufferCount];//right now just for backbuffering
ID3D12CommandAllocator* D3DClass::m_pCommandAllocator[g_cFrameBufferCount];
ID3D12Fence* D3DClass::m_pFence[g_cFrameBufferCount];
HANDLE D3DClass::m_hFenceEventHandle;
UINT64 D3DClass::m_ui64FenceValue[g_cFrameBufferCount];
unsigned int D3DClass::m_uiFrameIndex;
int D3DClass::m_iRTVDescriptorSize;

std::vector<ID3D12CommandList*> D3DClass::_vGraphicsCommandLists;


D3DClass::D3DClass()
{
}

D3DClass::~D3DClass()
{
	
}

bool D3DClass::Initialize(const unsigned int cFrameBufferCount, PSOHandler* pPsoHandler)
{
	HRESULT hr;

	#ifdef BUILD_ENABLE_D3D12_DEBUG
		ID3D12Debug* d;
		D3D12GetDebugInterface(IID_PPV_ARGS(&d));
		d->EnableDebugLayer();
		SAFE_RELEASE(d);
	#endif

	// -- Create the Device -- //

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		return false;
	}

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

	bool adapterFound = false; // set this to true when a good one was found

							   // find first hardware gpu that supports d3d 12
	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// we dont want a software device
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}

	if (!adapterFound)
	{
		return false;
	}

	// Create the device
	DxAssert(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice)), S_OK);
	

	// -- Create a direct command queue -- //

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	hr = m_pDevice->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_pCommandQueue)); // create the command queue
	if (FAILED(hr))
	{
		return false;
	}

	// -- Create the Swap Chain (double/tripple buffering) -- //

	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = WindowClass::GetWidth(); // buffer width
	backBufferDesc.Height = WindowClass::GetHeight(); // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

														// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

						  // Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = cFrameBufferCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = WindowClass::GetWindowHandler(); // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = !WindowClass::IsFullscreen(); // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain* tempSwapChain;

	// the queue will be flushed once the swap chain is created
	// give it the swap chain description we created above
	// store the created swap chain in a temp IDXGISwapChain interface
	dxgiFactory->CreateSwapChain(m_pCommandQueue, &swapChainDesc, &tempSwapChain);
	

	m_pSwapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

	m_uiFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //

	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = cFrameBufferCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

	// This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
	// otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DxAssert(m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRTVDescriptorHeap)), S_OK);

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	m_iRTVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < cFrameBufferCount; i++)
	{
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		DxAssert(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i])), S_OK);

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		m_pDevice->CreateRenderTargetView(m_pRenderTargets[i], nullptr, rtvHandle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtvHandle.Offset(1, m_iRTVDescriptorSize);
	}

	// -- Create the Command Allocators -- //

	for (int i = 0; i < cFrameBufferCount; i++)
	{
		DxAssert(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator[i])), S_OK);
	}

	// -- Create a Fence & Fence Event -- //

	// create the fences
	for (int i = 0; i < cFrameBufferCount; i++)
	{
		hr = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence[i]));
		if (FAILED(hr))
		{
			return false;
		}
		m_ui64FenceValue[i] = 0; // set the initial fence value to 0
	}

	// create a handle to a fence event
	m_hFenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_hFenceEventHandle == nullptr)
	{
		return false;
	}

	
	return true;
}

void D3DClass::Cleanup()
{
	// wait for the gpu to finish all frames
	
	for (int i = 0; i < g_cFrameBufferCount; i++)
	{
		m_uiFrameIndex = i;
		//WaitForPreviousFrame();
		if (m_pFence[m_uiFrameIndex]->GetCompletedValue() < m_ui64FenceValue[m_uiFrameIndex])
		{
			// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
			DxAssert(m_pFence[m_uiFrameIndex]->SetEventOnCompletion(m_ui64FenceValue[m_uiFrameIndex], m_hFenceEventHandle), S_OK);
			
			// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
			// has reached "fenceValue", we know the command queue has finished executing
			WaitForSingleObject(m_hFenceEventHandle, INFINITE);

		}
	}
	// get swapchain out of full screen before exiting
	BOOL fs = false;
	if (m_pSwapChain->GetFullscreenState(&fs, NULL))
		m_pSwapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pCommandQueue);
	SAFE_RELEASE(m_pRTVDescriptorHeap);

	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pRenderTargets[i]);
		SAFE_RELEASE(m_pCommandAllocator[i]);
		SAFE_RELEASE(m_pFence[i]);
	}

	_vGraphicsCommandLists.clear();
	
}

void D3DClass::WaitForPreviousFrame()
{
	HRESULT hr;

	// swap the current rtv buffer index so we draw on the correct buffer
	m_uiFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (m_pFence[m_uiFrameIndex]->GetCompletedValue() < m_ui64FenceValue[m_uiFrameIndex])
	{
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		DxAssert(m_pFence[m_uiFrameIndex]->SetEventOnCompletion(m_ui64FenceValue[m_uiFrameIndex], m_hFenceEventHandle), S_OK);
		
		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_hFenceEventHandle, INFINITE);
		
	}

	// increment fenceValue for next frame
	m_ui64FenceValue[m_uiFrameIndex]++;
}

ID3D12Device * D3DClass::GetDevice()
{
	return m_pDevice;
}

ID3D12CommandAllocator * D3DClass::GetCurrentCommandAllocator()
{
	return m_pCommandAllocator[m_uiFrameIndex];
}

ID3D12Resource * D3DClass::GetCurrentRenderTarget()
{
	return m_pRenderTargets[m_uiFrameIndex];
}

IDXGISwapChain3 * D3DClass::GetSwapChain()
{
	return m_pSwapChain;
}

ID3D12CommandQueue * D3DClass::GetCommandQueue()
{
	return m_pCommandQueue;
}

unsigned int D3DClass::GetFrameIndex()
{
	return m_uiFrameIndex;
}

int D3DClass::GetRTVDescriptorSize()
{
	return m_iRTVDescriptorSize;
}

ID3D12DescriptorHeap * D3DClass::GetRTVDescriptorHeap()
{
	return m_pRTVDescriptorHeap;
}

void D3DClass::IncrementFenceValue()
{
	m_ui64FenceValue[m_uiFrameIndex]++;
}

ID3D12Fence * D3DClass::GetCurrentFence()
{
	return m_pFence[m_uiFrameIndex];
}

UINT64 D3DClass::GetCurrentFenceValue()
{
	return m_ui64FenceValue[m_uiFrameIndex];
}

void D3DClass::QueueGraphicsCommandList(ID3D12CommandList* pCL)
{
	_vGraphicsCommandLists.push_back(pCL);
}

void D3DClass::ExecuteGraphicsCommandLists()
{
	// execute the array of command lists
	m_pCommandQueue->ExecuteCommandLists(_vGraphicsCommandLists.size(), _vGraphicsCommandLists.data());

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
	// queue is being executed on the GPU
	m_pCommandQueue->Signal(m_pFence[m_uiFrameIndex], m_ui64FenceValue[m_uiFrameIndex]);
	
	_vGraphicsCommandLists.clear();
}
