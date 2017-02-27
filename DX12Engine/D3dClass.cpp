#include "D3dClass.h"


D3dClass::D3dClass()
{
	m_pDevice = 0;
	m_pCommandQ = 0;
	m_pSwapChain = 0;
	m_pRtvHeap = 0;
	m_pBackbufferRenderTarget[0];
	m_pBackbufferRenderTarget[1];
	m_pCommandAllocator = 0;
	m_pGraphicsCommandList = 0;
	m_pPipelineState = 0;
	m_pFence = 0;
	m_fenceEvent = 0;
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

	m_bVsyncEnabled = vsyncstate;

	featureLevel = D3D_FEATURE_LEVEL_12_1;

	hr = D3D12CreateDevice(NULL, featureLevel, __uuidof(ID3D12Device), (void**)&m_pDevice);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Could not create a DirectX 12.1 device.  The default video card does not support DirectX 12.1.", L"DirectX Device Failure", MB_OK);
		
		return false;
	}

	//init commandq
	commandQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQDesc.NodeMask = 0;
	commandQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	hr = m_pDevice->CreateCommandQueue(&commandQDesc, __uuidof(ID3D12CommandQueue), (void**)&m_pCommandQ);
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
	for (i = 0; i < numModes; i++)
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

	error = wcstombs_s(&stringLength, m_chVideocardDesc, 128, adapterDesc.Description, 128);
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
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the refresh rate of the back buffer.
	if (m_bVsyncEnabled)
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
	hr = factory->CreateSwapChain(m_pCommandQ, &swapChainDesc, &p_tempSwapChain);
	if (FAILED(hr))
	{
		return false;
	}

	// Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_pSwapChain.
	// This will allow us to use the newer functionality such as getting the current back buffer index.
	hr = p_tempSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_pSwapChain);
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
	hr = m_pDevice->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pRtvHeap);
	if (FAILED(hr))
	{
		return false;
	}

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get a pointer to the first back buffer from the swap chain.
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&m_pBackbufferRenderTarget[0]);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a render target view for the first back buffer.
	m_pDevice->CreateRenderTargetView(m_pBackbufferRenderTarget[0], NULL, renderTargetViewHandle);

	// Increment the view handle to the next descriptor location in the render target view heap.
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;

	// Get a pointer to the second back buffer from the swap chain.
	hr = m_pSwapChain->GetBuffer(1, __uuidof(ID3D12Resource), (void**)&m_pBackbufferRenderTarget[1]);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a render target view for the second back buffer.
	m_pDevice->CreateRenderTargetView(m_pBackbufferRenderTarget[1], NULL, renderTargetViewHandle);

	// Finally get the initial index to which buffer is the current back buffer.
	m_iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// Create a command allocator.
	hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pCommandAllocator);
	if (FAILED(hr))
	{
		return false;
	}

	// Create a basic command list.
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pGraphicsCommandList);
	if (FAILED(hr))
	{
		return false;
	}

	// Initially we need to close the command list during initialization as it is created in a recording state.
	hr = m_pGraphicsCommandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	// Create a fence for GPU synchronization.
	hr = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pFence);
	if (FAILED(hr))
	{
		return false;
	}

	// Create an event object for the fence.
	m_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_fenceEvent == NULL)
	{
		return false;
	}

	// Initialize the starting fence value. 
	m_llFenceValue = 1;

	return true;
}

void D3dClass::Shutdown()
{
	int error;

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, NULL);
	}

	// Close the object handle to the fence event.
	error = CloseHandle(m_fenceEvent);
	if (error == 0)
	{
	}

	// Release the fence.
	if (m_pFence)
	{
		m_pFence->Release();
		m_pFence = 0;
	}

	// Release the empty pipe line state.
	if (m_pPipelineState)
	{
		m_pPipelineState->Release();
		m_pPipelineState = 0;
	}

	// Release the command list.
	if (m_pGraphicsCommandList)
	{
		m_pGraphicsCommandList->Release();
		m_pGraphicsCommandList = 0;
	}

	// Release the command allocator.
	if (m_pCommandAllocator)
	{
		m_pCommandAllocator->Release();
		m_pCommandAllocator = 0;
	}

	// Release the back buffer render target views.
	if (m_pBackbufferRenderTarget[0])
	{
		m_pBackbufferRenderTarget[0]->Release();
		m_pBackbufferRenderTarget[0] = 0;
	}
	if (m_pBackbufferRenderTarget[1])
	{
		m_pBackbufferRenderTarget[1]->Release();
		m_pBackbufferRenderTarget[1] = 0;
	}

	// Release the render target view heap.
	if (m_pRtvHeap)
	{
		m_pRtvHeap->Release();
		m_pRtvHeap = 0;
	}

	// Release the swap chain.
	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = 0;
	}

	// Release the command queue.
	if (m_pCommandQ)
	{
		m_pCommandQ->Release();
		m_pCommandQ = 0;
	}

	// Release the device.
	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = 0;
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
	hr = m_pCommandAllocator->Reset();
	if (FAILED(hr))
	{
		return false;
	}

	// Reset the command list, use empty pipeline state for now since there are no shaders and we are just clearing the screen.
	hr = m_pGraphicsCommandList->Reset(m_pCommandAllocator, m_pPipelineState);
	if (FAILED(hr))
	{
		return false;
	}

	// Record commands in the command list now.
	// Start by setting the resource barrier.
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pBackbufferRenderTarget[m_iBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_pGraphicsCommandList->ResourceBarrier(1, &barrier);

	// Get the render target view handle for the current back buffer.
	renderTargetViewHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (m_iBufferIndex == 1)
	{
		renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
	}

	// Set the back buffer as the render target.
	m_pGraphicsCommandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, NULL);

	// Then clear the window to.
	m_pGraphicsCommandList->ClearRenderTargetView(renderTargetViewHandle, clearColor, 0, NULL);

	// Indicate that the back buffer will now be used to present.
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_pGraphicsCommandList->ResourceBarrier(1, &barrier);

	// Close the list of commands.
	hr = m_pGraphicsCommandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	// Load the command list array (only one command list for now).
	ppCommandLists[0] = m_pGraphicsCommandList;

	// Execute the list of commands.
	m_pCommandQ->ExecuteCommandLists(1, ppCommandLists);

	// Finally present the back buffer to the screen since rendering is complete.
	if (m_bVsyncEnabled)
	{
		// Lock to screen refresh rate.
		hr = m_pSwapChain->Present(1, 0);
		if (FAILED(hr))
		{
			return false;
		}
	}
	else
	{
		// Present as fast as possible.
		hr = m_pSwapChain->Present(0, 0);
		if (FAILED(hr))
		{
			return false;
		}
	}

	// Signal and increment the fence value.
	fenceToWaitFor = m_llFenceValue;
	hr = m_pCommandQ->Signal(m_pFence, fenceToWaitFor);
	if (FAILED(hr))
	{
		return false;
	}
	m_llFenceValue++;

	// Wait until the GPU is done rendering.
	if (m_pFence->GetCompletedValue() < fenceToWaitFor)
	{
		hr = m_pFence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
		if (FAILED(hr))
		{
			return false;
		}
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	// Alternate the back buffer index back and forth between 0 and 1 each frame.
	m_iBufferIndex == 0 ? m_iBufferIndex = 1 : m_iBufferIndex = 0;

	return true;
}