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
	m_pRootSignature = 0;

	m_pFence = 0;
	m_fenceEvent = 0;
}

D3dClass::~D3dClass()
{
}

ID3D12Device * D3dClass::GetDevice()
{
	return m_pDevice;
}

bool D3dClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsyncstate, bool fullscreen)
{
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr;
	D3D12_COMMAND_QUEUE_DESC commandQDesc; 
	IDXGIFactory4* pFactory;
	IDXGIAdapter* pAdapter;
	IDXGIOutput* pAdapterOutput;
	unsigned int numModes, i, numerator, denominator, renderTargetViewDescriptorSize;
	unsigned long long stringLength;
	DXGI_MODE_DESC* pDisplayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	
	ID3DBlob* pRootSignatureBlob;

	ID3DBlob* pVertexShaderBlob;
	ID3DBlob* pPixelShaderBlob;
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	m_iFrameIndex = 0;


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
	
	DxAssert(m_pDevice->CreateCommandQueue(&commandQDesc, __uuidof(ID3D12CommandQueue), (void**)&m_pCommandQ), S_OK);

	DxAssert(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory), S_OK);

	DxAssert(pFactory->EnumAdapters(0, &pAdapter), S_OK);

	DxAssert(pAdapter->EnumOutputs(0, &pAdapterOutput), S_OK);

	DxAssert(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL), S_OK);
	
	pDisplayModeList = new DXGI_MODE_DESC[numModes];
	assert(pDisplayModeList);

	DxAssert(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, pDisplayModeList), S_OK);

	// Now go through all the display modes and find the one that matches the screen height and width.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i < numModes; i++)
	{
		if (pDisplayModeList[i].Height == (unsigned int)screenHeight)
		{
			if (pDisplayModeList[i].Width == (unsigned int)screenWidth)
			{
				numerator = pDisplayModeList[i].RefreshRate.Numerator;
				denominator = pDisplayModeList[i].RefreshRate.Denominator;
				break;
			}
		}
	}

	DxAssert(pAdapter->GetDesc(&adapterDesc), S_OK);
	

	error = wcstombs_s(&stringLength, m_chVideocardDesc, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}
	//release
	delete[] pDisplayModeList;
	SAFE_RELEASE(pAdapterOutput);
	SAFE_RELEASE(pAdapter);

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
	IDXGISwapChain* pTempSwapChain;
	DxAssert(pFactory->CreateSwapChain(m_pCommandQ, &swapChainDesc, &pTempSwapChain), S_OK);

	// Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_pSwapChain.
	// This will allow us to use the newer functionality such as getting the current back buffer index.
	DxAssert(pTempSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_pSwapChain), S_OK);

	// Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	pTempSwapChain = 0;

	// Release the factory now that the swap chain has been created.
	SAFE_RELEASE(pFactory);

	// Initialize the render target view heap description for the two back buffers.
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	// Set the number of descriptors to two for our two back buffers.  Also set the heap tyupe to render target views.
	renderTargetViewHeapDesc.NumDescriptors = 2;
	renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create the render target view heap for the back buffers.
	DxAssert(m_pDevice->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pRtvHeap), S_OK);

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get a pointer to the first back buffer from the swap chain.
	DxAssert(m_pSwapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&m_pBackbufferRenderTarget[0]), S_OK);

	// Create a render target view for the first back buffer.
	m_pDevice->CreateRenderTargetView(m_pBackbufferRenderTarget[0], NULL, renderTargetViewHandle);

	// Increment the view handle to the next descriptor location in the render target view heap.
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;

	// Get a pointer to the second back buffer from the swap chain.
	DxAssert(m_pSwapChain->GetBuffer(1, __uuidof(ID3D12Resource), (void**)&m_pBackbufferRenderTarget[1]), S_OK);

	// Create a render target view for the second back buffer.
	m_pDevice->CreateRenderTargetView(m_pBackbufferRenderTarget[1], NULL, renderTargetViewHandle);

	// Finally get the initial index to which buffer is the current back buffer.
	m_iBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// Create a command allocator.
	DxAssert(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pCommandAllocator), S_OK);

	// Create a basic command list.
	DxAssert(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pGraphicsCommandList), S_OK);

	// Initially we need to close the command list during initialization as it is created in a recording state.
	DxAssert(m_pGraphicsCommandList->Close(), S_OK);

	// Create a fence for GPU synchronization.
	DxAssert(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pFence), S_OK);

	// Create an event object for the fence.
	m_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_fenceEvent == NULL)
	{
		return false;
	}

	// Initialize the starting fence value. 
	m_llFenceValue = 1;



	//TODO: Move this 
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pParameters = nullptr;
	rootSignatureDesc.pStaticSamplers = nullptr;

	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pRootSignatureBlob, nullptr), S_OK);

	DxAssert(m_pDevice->CreateRootSignature(0, pRootSignatureBlob->GetBufferPointer(), pRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);
	
	DxAssert(D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pVertexShaderBlob, nullptr), S_OK);
	
	vertexShaderBytecode.BytecodeLength = pVertexShaderBlob->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = pVertexShaderBlob->GetBufferPointer();

	DxAssert(D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pPixelShaderBlob, nullptr), S_OK);
	
	pixelShaderBytecode.BytecodeLength = pPixelShaderBlob->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pPixelShaderBlob->GetBufferPointer();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	D3D12_INPUT_LAYOUT_DESC testLayout;


	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;

	D3D12_BLEND_DESC blendDesc = {};


	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = vertexShaderBytecode;
	psoDesc.PS = pixelShaderBytecode;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = swapChainDesc.SampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.NumRenderTargets = 1;

	DxAssert(m_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineState)), S_OK);
	Vertex vList[] = {
		{ { 0.0f, 0.5f, 0.5f } },
		{ { 0.5f, -0.5f, 0.5f } },
		{ { -0.5f, -0.5f, 0.5f } },
	};

	int vBufferSize = sizeof(vList);

	DxAssert(m_pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&testVertexBuffer)), S_OK);

	testVertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	ID3D12Resource* vBufferUploadHeap;

	m_pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vBufferUploadHeap));

	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList);
	vertexData.RowPitch = vBufferSize;
	vertexData.SlicePitch = vBufferSize;

	UpdateSubresources(m_pGraphicsCommandList, testVertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	m_pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(testVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	m_pGraphicsCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { m_pGraphicsCommandList };
	m_pCommandQ->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	DxAssert(m_pCommandQ->Signal(m_pFence, m_llFenceValue), S_OK);

	vertexBufferView.BufferLocation = testVertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vBufferSize;

	// Fill out the Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = screenWidth;
	scissorRect.bottom = screenHeight;

	m_pGraphicsCommandList->SetGraphicsRootSignature(m_pRootSignature); // set the root signature
	m_pGraphicsCommandList->RSSetViewports(1, &viewport); // set the viewports
	m_pGraphicsCommandList->RSSetScissorRects(1, &scissorRect); // set the scissor rects
	m_pGraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	m_pGraphicsCommandList->IASetVertexBuffers(0, 1, &vertexBufferView); // set the vertex buffer (using the vertex buffer view)
	m_pGraphicsCommandList->DrawInstanced(3, 1, 0, 0); // finally draw 3 vertices (draw the triangle)

	DxAssert(m_pSwapChain->Present(0, 0), S_OK);

	int stopper = 0;

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
	SAFE_RELEASE(m_pFence);

	// Release the empty pipe line state.
	SAFE_RELEASE(m_pPipelineState);
	

	// Release the command list.
	SAFE_RELEASE(m_pGraphicsCommandList);

	// Release the command allocator.
	SAFE_RELEASE(m_pCommandAllocator);
	

	// Release the back buffer render target views.
	SAFE_RELEASE(m_pBackbufferRenderTarget[0]);
	SAFE_RELEASE(m_pBackbufferRenderTarget[1]);
	
	// Release the render target view heap.
	SAFE_RELEASE(m_pRtvHeap);
		
	// Release the swap chain.
	SAFE_RELEASE(m_pSwapChain);
	
	// Release the command queue.
	SAFE_RELEASE(m_pCommandQ);
	
	// Release the device.
	SAFE_RELEASE(m_pDevice);
	
	return;
}

bool D3dClass::Render()
{
	D3D12_RESOURCE_BARRIER barrier;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	unsigned int renderTargetViewDescriptorSize;
	float clearColor[4]{ 0.5f, 0.0f, 0.0f, 1.0f };
	ID3D12CommandList* ppCommandLists[1];
	unsigned long long fenceToWaitFor;

	// Reset (re-use) the memory associated command allocator.
	DxAssert(m_pCommandAllocator->Reset(), S_OK);
	

	// Reset the command list, use empty pipeline state for now since there are no shaders and we are just clearing the screen.
	DxAssert(m_pGraphicsCommandList->Reset(m_pCommandAllocator, m_pPipelineState), S_OK);

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
	DxAssert(m_pGraphicsCommandList->Close(), S_OK);

	// Load the command list array (only one command list for now).
	ppCommandLists[0] = m_pGraphicsCommandList;

	// Execute the list of commands.
	m_pCommandQ->ExecuteCommandLists(1, ppCommandLists);

	// Finally present the back buffer to the screen since rendering is complete.
	if (m_bVsyncEnabled)
	{
		// Lock to screen refresh rate.
		DxAssert(m_pSwapChain->Present(1, 0), S_OK);
		
	}
	else
	{
		// Present as fast as possible.
		DxAssert(m_pSwapChain->Present(0, 0), S_OK);
		
	}

	// Signal and increment the fence value.
	fenceToWaitFor = m_llFenceValue;
	DxAssert(m_pCommandQ->Signal(m_pFence, fenceToWaitFor), S_OK);

	m_llFenceValue++;

	// Wait until the GPU is done rendering.
	if (m_pFence->GetCompletedValue() < fenceToWaitFor)
	{
		DxAssert(m_pFence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent), S_OK);
		
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	// Alternate the back buffer index back and forth between 0 and 1 each frame.
	m_iBufferIndex == 0 ? m_iBufferIndex = 1 : m_iBufferIndex = 0;

	return true;
}