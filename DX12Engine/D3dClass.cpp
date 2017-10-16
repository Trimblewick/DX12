
#include "D3dClass.h"



ID3D12Device* D3DClass::s_pDevice;
IDXGISwapChain3* D3DClass::s_pSwapChain;
ID3D12CommandQueue* D3DClass::s_pCommandQueue;

ID3D12Fence* D3DClass::s_pFenceCQ[g_iBackBufferCount];
HANDLE D3DClass::s_hFenceEventHandle;
UINT64 D3DClass::s_ui64FenceValue[g_iBackBufferCount];
unsigned int D3DClass::s_uiFrameIndex;
int D3DClass::s_iRTVDescriptorSize;

D3D12_CPU_DESCRIPTOR_HANDLE* D3DClass::s_pRTVHandle;
std::vector<ID3D12CommandList*> D3DClass::_pGraphicsCommandLists;
IDXGIFactory4* D3DClass::s_pDXGIFactory;


D3DClass::D3DClass()
{
}

D3DClass::~D3DClass()
{
	
}

bool D3DClass::Initialize()
{
	HRESULT hr;

	#ifdef BUILD_ENABLE_D3D12_DEBUG
		ID3D12Debug* d;
		D3D12GetDebugInterface(IID_PPV_ARGS(&d));
		d->EnableDebugLayer();
		SAFE_RELEASE(d);
	#endif

	// -- Create the Device -- //

	
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&s_pDXGIFactory));
	if (FAILED(hr))
	{
		return false;
	}

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

	bool adapterFound = false; // set this to true when a good one was found

							   // find first hardware gpu that supports d3d 12
	while (s_pDXGIFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
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
	DxAssert(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_pDevice)), S_OK);
	

	// -- Create a direct command queue -- //
	/*
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	hr = s_pDevice->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&s_pCommandQueue)); // create the command queue
	if (FAILED(hr))
	{
		return false;
	}*/

	// -- Create the Swap Chain (double/tripple buffering) -- //
	/*
	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = WindowClass::GetWidth(); // buffer width
	backBufferDesc.Height = WindowClass::GetHeight(); // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

														// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

						  // Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = g_iBackBufferCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = WindowClass::GetWindowHandler(); // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = !WindowClass::IsFullscreen(); // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain* tempSwapChain;*/

	// the queue will be flushed once the swap chain is created
	// give it the swap chain description we created above
	// store the created swap chain in a temp IDXGISwapChain interface
	
	//dxgiFactory->CreateSwapChain(s_pCommandQueue, &swapChainDesc, &tempSwapChain);
	//s_pSwapChain = CreateSwapChain(&swapChainDesc, s_pCommandQueue);

	

	s_uiFrameIndex = 0;// s_pSwapChain->GetCurrentBackBufferIndex();

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //
	/*
	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = g_iBackBufferCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

	// This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
	// otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DxAssert(s_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&s_pRTVDescriptorHeap)), S_OK);

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	s_iRTVDescriptorSize = s_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(s_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		DxAssert(s_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&s_pRenderTargets[i])), S_OK);

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		s_pDevice->CreateRenderTargetView(s_pRenderTargets[i], nullptr, rtvHandle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtvHandle.Offset(1, s_iRTVDescriptorSize);
	}
	*/
	// -- Create the Command Allocators -- //
	/*
	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		DxAssert(s_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&s_pCommandAllocator[i])), S_OK);
	}
	*/
	// -- Create a Fence & Fence Event -- //

	// create the fences
	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		hr = s_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&s_pFenceCQ[i]));
		if (FAILED(hr))
		{
			return false;
		}
		s_ui64FenceValue[i] = 0; // set the initial fence value to 0
	}

	// create a handle to a fence event
	s_hFenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (s_hFenceEventHandle == nullptr)
	{
		return false;
	}

	s_pRTVHandle = new D3D12_CPU_DESCRIPTOR_HANDLE();
	
	return true;
}

void D3DClass::Cleanup()
{
	// wait for the gpu to finish all frames
	s_ui64FenceValue[s_uiFrameIndex]--;//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<---------------------
	for (int i = 0; i < g_iBackBufferCount; i++)
	{
		s_uiFrameIndex = i;
		//WaitForPreviousFrame();
		if (s_pFenceCQ[s_uiFrameIndex]->GetCompletedValue() < s_ui64FenceValue[s_uiFrameIndex])
		{
			// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
			DxAssert(s_pFenceCQ[s_uiFrameIndex]->SetEventOnCompletion(s_ui64FenceValue[s_uiFrameIndex], s_hFenceEventHandle), S_OK);
			
			// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
			// has reached "fenceValue", we know the command queue has finished executing
			WaitForSingleObject(s_hFenceEventHandle, 3000);

		}
	}
	// get swapchain out of full screen before exiting
	BOOL fs = false;
	if (s_pSwapChain->GetFullscreenState(&fs, NULL))
		s_pSwapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(s_pDevice);
	SAFE_RELEASE(s_pSwapChain);
	SAFE_RELEASE(s_pCommandQueue);
	

	for (int i = 0; i < g_iBackBufferCount; ++i)
	{

		
		SAFE_RELEASE(s_pFenceCQ[i]);
	}
	if (s_pRTVHandle)
	{
		delete s_pRTVHandle;
		s_pRTVHandle = nullptr;
	}
	//WIERDEST LEAK EUW?
	//std::vector <ID3D12CommandList*> temp;
	_pGraphicsCommandLists.clear();
	//_vGraphicsCommandLists.swap(temp);
	_pGraphicsCommandLists.shrink_to_fit();
	
}

void D3DClass::WaitForPreviousFrame()//wait for the command Q to finish on the gpu
{
	HRESULT hr;

	// swap the current rtv buffer index so we draw on the correct buffer
	s_uiFrameIndex = s_pSwapChain->GetCurrentBackBufferIndex();

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (s_pFenceCQ[s_uiFrameIndex]->GetCompletedValue() < s_ui64FenceValue[s_uiFrameIndex])
	{
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		DxAssert(s_pFenceCQ[s_uiFrameIndex]->SetEventOnCompletion(s_ui64FenceValue[s_uiFrameIndex], s_hFenceEventHandle), S_OK);
		
		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(s_hFenceEventHandle, INFINITE);
		
	}

	// increment fenceValue for next frame
	s_ui64FenceValue[s_uiFrameIndex]++;


	//DxAssert(s_pCommandAllocator[s_uiFrameIndex]->Reset(), S_OK);
}

ID3D12CommandAllocator * D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE listType)
{
	ID3D12CommandAllocator* pCA;
	DxAssert(s_pDevice->CreateCommandAllocator(listType, IID_PPV_ARGS(&pCA)), S_OK);
	return pCA;
}

ID3D12Fence * D3DClass::CreateFence(UINT64 ui64InitVal, D3D12_FENCE_FLAGS fenceFlag)
{
	ID3D12Fence* pFence;
	DxAssert(s_pDevice->CreateFence(ui64InitVal, fenceFlag, IID_PPV_ARGS(&pFence)), S_OK);
	return pFence;
}

ID3D12DescriptorHeap * D3DClass::CreateDH(int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	ID3D12DescriptorHeap* pDH;
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	DxAssert(s_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDH)), S_OK);
	return pDH;
}

ID3D12GraphicsCommandList * D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE listType, ID3D12CommandAllocator* pCA)
{
	ID3D12GraphicsCommandList* pCL;
	DxAssert(s_pDevice->CreateCommandList(0, listType, pCA, nullptr, IID_PPV_ARGS(&pCL)), S_OK);
	return pCL;
}

ID3D12CommandQueue* D3DClass::CreateCQ(D3D12_COMMAND_LIST_TYPE listType)
{
	ID3D12CommandQueue* pCQ;

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = listType;

	DxAssert(s_pDevice->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&pCQ)), S_OK);

	return pCQ;
}

IDXGISwapChain3 * D3DClass::CreateSwapChain(DXGI_SWAP_CHAIN_DESC* desc, ID3D12CommandQueue * pCQ)
{
	IDXGISwapChain* pTemp = nullptr;
	IDXGISwapChain3* pSwapChain = nullptr;

	HRESULT hr = s_pDXGIFactory->CreateSwapChain(pCQ, desc, &pTemp);

	pSwapChain = static_cast<IDXGISwapChain3*>(pTemp);

	return pSwapChain;
}

void D3DClass::temp_setsw(IDXGISwapChain3 * pSW)
{
	s_pSwapChain = pSW;
}

ID3D12Device * D3DClass::GetDevice()
{
	return s_pDevice;
}

IDXGISwapChain3 * D3DClass::GetSwapChain()
{
	return s_pSwapChain;
}



unsigned int D3DClass::GetFrameIndex()
{
	return s_pSwapChain->GetCurrentBackBufferIndex();
}


void D3DClass::IncrementFenceValue()
{
	s_ui64FenceValue[s_uiFrameIndex]++;
}

ID3D12Fence * D3DClass::GetCurrentFence()
{
	return s_pFenceCQ[s_uiFrameIndex];
}

UINT64 D3DClass::GetCurrentFenceValue()
{
	return s_ui64FenceValue[s_uiFrameIndex];
}
