#include "TriangleObject.h"





TriangleObject::TriangleObject(PSOHandler* pPsoHandler)
{
	HRESULT hr;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementDesc[] = { 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, 
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc;

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayoutElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayoutElementDesc;

	

	D3DClass::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3DClass::GetCurrentCommandAllocator(), NULL, IID_PPV_ARGS(&m_pCommandList));
	
	
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	

	ID3DBlob* signature;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr), S_OK);
	
	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);

	
	//get the swapchaindesc to get its sample desc into the pso
	D3DClass::GetSwapChain()->GetDesc(&tempSwapChainDesc);

	

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
		return;
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
		return;
	}

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();


	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = m_pRootSignature; // the root signature that describes the input data this pso needs
	psoDesc.VS = vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = tempSwapChainDesc.SampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target

	// create the pso
	_pPSO = pPsoHandler->CreatePipelineStateObject(&psoDesc, D3DClass::GetDevice());

	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);

	//for a quad
	Vertex v1;
	v1.pos = { -0.5f, 0.5f, 0.5f };
	v1.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	Vertex v2;
	v2.pos = { 0.5f, -0.5f, 0.5f };
	v2.color = { 0.0f, 1.0f, 0.0f, 1.0f };
	Vertex v3;
	v3.pos = { -0.5f, -0.5f, 0.5f };
	v3.color = { 0.0f, 0.0f, 1.0f, 1.0f };
	Vertex v4;
	v4.pos = { 0.5f, 0.5f, 0.5f };
	v4.color = { 1.0f, 0.0f, 1.0f, 1.0f };
	
	//vertex list
	Vertex vList[] = { v1, v2, v3, v4 };

	
	int vBufferSize = sizeof(vList);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&m_pVertexBuffer)), S_OK);

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_pVertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* pVertexBufferUploadHeap;
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&pVertexBufferUploadHeap)), S_OK);
	pVertexBufferUploadHeap->SetName(L"Vertex Buffer Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

										 // we are now creating a command with the command list to copy the data from
										 // the upload heap to the default heap
	UpdateSubresources(m_pCommandList, m_pVertexBuffer, pVertexBufferUploadHeap, 0, 0, 1, &vertexData);

	
	
	// transition the vertex buffer data from copy destination state to vertex buffer state
	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	//index  list
	DWORD iList[] = { 0, 1, 2, 0, 3, 1 };

	int iBufferSize = sizeof(iList);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer)), S_OK);

	m_pIndexBuffer->SetName(L"Index Buffer Resource Heap");

	ID3D12Resource* pIndexBufferUploadHeap;
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, 
		&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pIndexBufferUploadHeap)), S_OK);
	
	pIndexBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");
	
	D3D12_SUBRESOURCE_DATA indexData{};
	indexData.pData = reinterpret_cast<BYTE*>(iList);
	indexData.RowPitch = iBufferSize;
	indexData.SlicePitch = iBufferSize;

	UpdateSubresources(m_pCommandList, m_pIndexBuffer, pIndexBufferUploadHeap, 0, 0, 1, &indexData);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


	// Now we execute the command list to upload the initial assets (triangle data)
	DxAssert(m_pCommandList->Close(), S_OK);

	
	
	D3DClass::QueueGraphicsCommandList(m_pCommandList);
	//D3DClass::IncrementFenceValue();
	D3DClass::ExecuteGraphicsCommandLists();
	
	
	SAFE_RELEASE(pVertexBufferUploadHeap);//Should this be released upon exiting??!
	SAFE_RELEASE(pIndexBufferUploadHeap);
	
	
	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	
	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = vBufferSize;

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = iBufferSize;
}

TriangleObject::~TriangleObject()
{
	SAFE_RELEASE(m_pCommandList);
	
	SAFE_RELEASE(m_pRootSignature);
	SAFE_RELEASE(m_pVertexBuffer);

	SAFE_RELEASE(m_pIndexBuffer);

	_pPSO = nullptr;
}

void TriangleObject::Draw(D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, Camera* camera)
{
	DxAssert(m_pCommandList->Reset(D3DClass::GetCurrentCommandAllocator(), _pPSO), S_OK);
	
	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(D3DClass::GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	m_pCommandList->OMSetRenderTargets(1, rtvHandle, FALSE, nullptr);


	const float clearColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	m_pCommandList->ClearRenderTargetView(*rtvHandle, clearColor, 0, nullptr);


	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);
	m_pCommandList->RSSetViewports(1, &camera->GetViewport());
	m_pCommandList->RSSetScissorRects(1, &camera->GetScissorRect());
	m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_pCommandList->IASetIndexBuffer(&m_indexBufferView);
	m_pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(D3DClass::GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	DxAssert(m_pCommandList->Close(), S_OK);
	

	return;
}

ID3D12CommandList * TriangleObject::GetCommandList()
{
	return m_pCommandList;
}


