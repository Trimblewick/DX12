#include "D3dClass.h"


D3dClass::D3dClass()
{
	this->device = 0;
	this->commandQ = 0;
	this->swapChain = 0;
	this->rtvHeap = 0;
	this->backbufferRenderTarget[0];
	this->backbufferRenderTarget[1];
	this->commandAllocator = 0;
	this->graphicsCommandList = 0;
	this->pipelineState = 0;
	this->fence = 0;
	this->fenceEvent = 0;
}

D3dClass::~D3dClass()
{
}

bool D3dClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsyncstate, bool fullscreen)
{
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr;
	D3D12_COMMAND_QUEUE_DESC commandQDesc; 
	IDXGIFactory4* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, renderTargetViewDescriptorSize;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;

	this->vsync_enabled = vsyncstate;

	featureLevel = D3D_FEATURE_LEVEL_12_1;

	hr = D3D12CreateDevice(NULL, featureLevel, __uuidof(ID3D12Device), (void**)&this->device);
	if (FAILED(hr))
	{
		//MessageBox(hwnd, L"Could not create a DirectX 12.1 device.  The default video card does not support DirectX 12.1.", L"DirectX Device Failure", MB_OK);
		//throw exception?
		
		return false;
	}

	//init commandq
	commandQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQDesc.NodeMask = 0;
	commandQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	hr = this->device->CreateCommandQueue(&commandQDesc, __uuidof(ID3D12CommandQueue), (void**)&this->commandQ);
	if (FAILED(hr))
	{
		return false;
	}

	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
	if (FAILED(hr))
	{
		return false;
	}

	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		return false;
	}

	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr))
	{
		return false;
	}

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(hr))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen height and width.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	bool test = false;
	bool test2 = false;
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Height == (unsigned int)screenHeight)
		{
			test = true;
			if (displayModeList[i].Width == (unsigned int)screenWidth)
			{
				test2 = true;
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
	{
		return false;
	}

	error = wcstombs_s(&stringLength, this->videocardDesc, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}
	//release
	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = hwnd;
	/*if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{*/
		swapChainDesc.Windowed = true;
	//}

	// Set the refresh rate of the back buffer.
	if (this->vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Finally create the swap chain using the swap chain description.	
	IDXGISwapChain* p_tempSwapChain;
	hr = factory->CreateSwapChain(this->commandQ, &swapChainDesc, &p_tempSwapChain);
	if (FAILED(hr))
	{
		return false;
	}

	// Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
	// This will allow us to use the newer functionality such as getting the current back buffer index.
	hr = p_tempSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&this->swapChain);
	if (FAILED(hr))
	{
		return false;
	}

	// Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	p_tempSwapChain = 0;

	// Release the factory now that the swap chain has been created.
	factory->Release();
	factory = 0;


	// Initialize the render target view heap description for the two back buffers.
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	// Set the number of descriptors to two for our two back buffers.  Also set the heap tyupe to render target views.
	renderTargetViewHeapDesc.NumDescriptors = 2;
	renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create the render target view heap for the back buffers.
	hr = this->device->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&this->rtvHeap);
	if (FAILED(hr))
	{
		return false;
	}

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = this->rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = this->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get a pointer to the first back buffer from the swap chain.
	hr = this->swapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&this->backbufferRenderTarget[0]);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a render target view for the first back buffer.
	this->device->CreateRenderTargetView(this->backbufferRenderTarget[0], NULL, renderTargetViewHandle);

	// Increment the view handle to the next descriptor location in the render target view heap.
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;

	// Get a pointer to the second back buffer from the swap chain.
	hr = this->swapChain->GetBuffer(1, __uuidof(ID3D12Resource), (void**)&this->backbufferRenderTarget[1]);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a render target view for the second back buffer.
	this->device->CreateRenderTargetView(this->backbufferRenderTarget[1], NULL, renderTargetViewHandle);

	// Finally get the initial index to which buffer is the current back buffer.
	this->bufferIndex = this->swapChain->GetCurrentBackBufferIndex();

	// Create a command allocator.
	hr = this->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&this->commandAllocator);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a basic command list.
	hr = this->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&this->graphicsCommandList);
	if (FAILED(hr))
	{
		return false;
	}

	// Initially we need to close the command list during initialization as it is created in a recording state.
	hr = this->graphicsCommandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	// Create a fence for GPU synchronization.
	hr = this->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&this->fence);
	if (FAILED(hr))
	{
		return false;
	}

	// Create an event object for the fence.
	this->fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (this->fenceEvent == NULL)
	{
		return false;
	}

	// Initialize the starting fence value. 
	this->fenceValue = 1;


	return true;
}

void D3dClass::Shutdown()
{
	int error;

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (this->swapChain)
	{
		this->swapChain->SetFullscreenState(false, NULL);
	}

	// Close the object handle to the fence event.
	error = CloseHandle(this->fenceEvent);
	if (error == 0)
	{
	}

	// Release the fence.
	if (this->fence)
	{
		this->fence->Release();
		this->fence = 0;
	}

	// Release the empty pipe line state.
	if (this->pipelineState)
	{
		this->pipelineState->Release();
		this->pipelineState = 0;
	}

	// Release the command list.
	if (this->graphicsCommandList)
	{
		this->graphicsCommandList->Release();
		this->graphicsCommandList = 0;
	}

	// Release the command allocator.
	if (this->commandAllocator)
	{
		this->commandAllocator->Release();
		this->commandAllocator = 0;
	}

	// Release the back buffer render target views.
	if (this->backbufferRenderTarget[0])
	{
		this->backbufferRenderTarget[0]->Release();
		this->backbufferRenderTarget[0] = 0;
	}
	if (this->backbufferRenderTarget[1])
	{
		this->backbufferRenderTarget[1]->Release();
		this->backbufferRenderTarget[1] = 0;
	}

	// Release the render target view heap.
	if (this->rtvHeap)
	{
		this->rtvHeap->Release();
		this->rtvHeap = 0;
	}

	// Release the swap chain.
	if (this->swapChain)
	{
		this->swapChain->Release();
		this->swapChain = 0;
	}

	// Release the command queue.
	if (this->commandQ)
	{
		this->commandQ->Release();
		this->commandQ = 0;
	}

	// Release the device.
	if (this->device)
	{
		this->device->Release();
		this->device = 0;
	}

	return;
}

bool D3dClass::Render()
{
	HRESULT hr;
	D3D12_RESOURCE_BARRIER barrier;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	unsigned int renderTargetViewDescriptorSize;
	float clearColor[4]{ 0.5f, 0.0f, 0.0f, 1.0f };
	ID3D12CommandList* ppCommandLists[1];
	unsigned long long fenceToWaitFor;

	// Reset (re-use) the memory associated command allocator.
	hr = this->commandAllocator->Reset();
	if (FAILED(hr))
	{
		return false;
	}

	// Reset the command list, use empty pipeline state for now since there are no shaders and we are just clearing the screen.
	hr = this->graphicsCommandList->Reset(this->commandAllocator, this->pipelineState);
	if (FAILED(hr))
	{
		return false;
	}

	// Record commands in the command list now.
	// Start by setting the resource barrier.
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = this->backbufferRenderTarget[this->bufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	this->graphicsCommandList->ResourceBarrier(1, &barrier);

	// Get the render target view handle for the current back buffer.
	renderTargetViewHandle = this->rtvHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescriptorSize = this->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (this->bufferIndex == 1)
	{
		renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
	}

	// Set the back buffer as the render target.
	this->graphicsCommandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, NULL);

	// Then clear the window to.
	this->graphicsCommandList->ClearRenderTargetView(renderTargetViewHandle, clearColor, 0, NULL);

	// Indicate that the back buffer will now be used to present.
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	this->graphicsCommandList->ResourceBarrier(1, &barrier);

	// Close the list of commands.
	hr = this->graphicsCommandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	// Load the command list array (only one command list for now).
	ppCommandLists[0] = this->graphicsCommandList;

	// Execute the list of commands.
	this->commandQ->ExecuteCommandLists(1, ppCommandLists);

	// Finally present the back buffer to the screen since rendering is complete.
	if (this->vsync_enabled)
	{
		// Lock to screen refresh rate.
		hr = this->swapChain->Present(1, 0);
		if (FAILED(hr))
		{
			return false;
		}
	}
	else
	{
		// Present as fast as possible.
		hr = this->swapChain->Present(0, 0);
		if (FAILED(hr))
		{
			return false;
		}
	}

	// Signal and increment the fence value.
	fenceToWaitFor = this->fenceValue;
	hr = this->commandQ->Signal(this->fence, fenceToWaitFor);
	if (FAILED(hr))
	{
		return false;
	}
	this->fenceValue++;

	// Wait until the GPU is done rendering.
	if (this->fence->GetCompletedValue() < fenceToWaitFor)
	{
		hr = this->fence->SetEventOnCompletion(fenceToWaitFor, this->fenceEvent);
		if (FAILED(hr))
		{
			return false;
		}
		WaitForSingleObject(this->fenceEvent, INFINITE);
	}

	// Alternate the back buffer index back and forth between 0 and 1 each frame.
	this->bufferIndex == 0 ? this->bufferIndex = 1 : this->bufferIndex = 0;

	return true;
}