
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

/*
//temp
 ID3D12GraphicsCommandList* D3DClass::commandList; // a command list we can record commands into, then execute them to render the frame

 ID3D12PipelineState* D3DClass::pipelineStateObject; // pso containing a pipeline state

 ID3D12RootSignature* D3DClass::rootSignature; // root signature defines data shaders will access

 ID3D12Resource* D3DClass::vertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into

 D3D12_VERTEX_BUFFER_VIEW D3DClass::vertexBufferView; // a structure containing a pointer to the vertex data in gpu memory

 */
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

	// -- Create a Command List -- //

	// create the command list with the first allocator
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator[m_uiFrameIndex], NULL, IID_PPV_ARGS(&commandList));
	if (FAILED(hr))
	{
		return false;
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

	// create root signature

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(hr))
	{
		return false;
	}

	// create vertex and pixel shaders

	// when debugging, we can compile the shader files at runtime.
	// but for release versions, we can compile the hlsl shaders
	// with fxc.exe to create .cso files, which contain the shader
	// bytecode. We can load the .cso files at runtime to get the
	// shader bytecode, which of course is faster than compiling
	// them at runtime

	// compile vertex shader
	ID3DBlob* vertexShader; // d3d blob for holding vertex shader bytecode
	ID3DBlob* errorBuff; // a buffer holding the error data if any
	hr = D3DCompileFromFile(L"VertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShader,
		&errorBuff);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return false;
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();

	// compile pixel shader
	ID3DBlob* pixelShader;
	hr = D3DCompileFromFile(L"PixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShader,
		&errorBuff);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return false;
	}

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

	// create input layout

	// The input layout is used by the Input Assembler so that it knows
	// how to read the vertex data bound to it.

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	// create a pipeline state object (PSO)

	// In a real application, you will have many pso's. for each different shader
	// or different combinations of shaders, different blend states or different rasterizer states,
	// different topology types (point, line, triangle, patch), or a different number
	// of render targets you will need a pso

	// VS is the only required shader for a pso. You might be wondering when a case would be where
	// you only set the VS. It's possible that you have a pso that only outputs data with the stream
	// output, and not on a render target, which means you would not need anything after the stream
	// output.

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = rootSignature; // the root signature that describes the input data this pso needs
	psoDesc.VS = vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target

	// create the pso

	pipelineStateObject = pPsoHandler->CreatePipelineStateObject(&psoDesc, m_pDevice);
	/*
	hr = m_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStateObject));
	if (FAILED(hr))
	{
		return false;
	}
	*/
	// Create vertex buffer

	// a triangle
	Vertex vList[] = {
		{ { 0.0f, 0.5f, 0.5f } },
		{ { 0.5f, -0.5f, 0.5f } },
		{ { -0.5f, -0.5f, 0.5f } },
	};

	int vBufferSize = sizeof(vList);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	m_pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&vertexBuffer));

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	vertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;
	m_pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));
	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

										 // we are now creating a command with the command list to copy the data from
										 // the upload heap to the default heap
	UpdateSubresources(commandList, vertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// Now we execute the command list to upload the initial assets (triangle data)
	commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { commandList };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	m_ui64FenceValue[m_uiFrameIndex]++;
	hr = m_pCommandQueue->Signal(m_pFence[m_uiFrameIndex], m_ui64FenceValue[m_uiFrameIndex]);
	if (FAILED(hr))
	{
		//Running = false;
	}

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vBufferSize;

	return true;
}

bool D3DClass::Render(Camera* camera)
{
	HRESULT hr;

	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	hr = m_pCommandAllocator[m_uiFrameIndex]->Reset();
	if (FAILED(hr))
	{
		return false;
	}

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	hr = commandList->Reset(m_pCommandAllocator[m_uiFrameIndex], pipelineStateObject);
	if (FAILED(hr))
	{
		return false;
	}

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_uiFrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_uiFrameIndex, m_iRTVDescriptorSize);

	// set the render target for the output merger stage (the output of the pipeline)
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.5f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// draw triangle
	commandList->SetGraphicsRootSignature(rootSignature); // set the root signature
	commandList->RSSetViewports(1, &camera->GetViewport()); // set the viewports
	commandList->RSSetScissorRects(1, &camera->GetScissorRect()); // set the scissor rects
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView); // set the vertex buffer (using the vertex buffer view)
	commandList->DrawInstanced(3, 1, 0, 0); // finally draw 3 vertices (draw the triangle)

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_uiFrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	hr = commandList->Close();
	if (FAILED(hr))
	{
		return false;
	}

	ID3D12CommandList* ppCommandLists[] = { commandList };

	// execute the array of command lists
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
	// queue is being executed on the GPU
	hr = m_pCommandQueue->Signal(m_pFence[m_uiFrameIndex], m_ui64FenceValue[m_uiFrameIndex]);
	if (FAILED(hr))
	{
		return false;
	}
	// present the current backbuffer
	hr = m_pSwapChain->Present(0, 0);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void D3DClass::Cleanup()
{
	// wait for the gpu to finish all frames
	
	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		//WaitForPreviousFrame();
	}

	// get swapchain out of full screen before exiting
	BOOL fs = false;
	if (m_pSwapChain->GetFullscreenState(&fs, NULL))
		m_pSwapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pCommandQueue);
	SAFE_RELEASE(m_pRTVDescriptorHeap);
	SAFE_RELEASE(commandList);

	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pRenderTargets[i]);
		SAFE_RELEASE(m_pCommandAllocator[i]);
		SAFE_RELEASE(m_pFence[i]);
	};

	SAFE_RELEASE(pipelineStateObject);
	SAFE_RELEASE(rootSignature);
	SAFE_RELEASE(vertexBuffer);

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
		hr = m_pFence[m_uiFrameIndex]->SetEventOnCompletion(m_ui64FenceValue[m_uiFrameIndex], m_hFenceEventHandle);
		if (FAILED(hr))
		{
			int stopper = 0;
			//Running = false;
		}
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
