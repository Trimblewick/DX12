#include "BoxObject.h"


BoxObject::BoxObject(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 rot, PSOHandler* pPsoHandler)
{
	HRESULT hr;
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayoutElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayoutElementDesc;

	D3DClass::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3DClass::GetCurrentCommandAllocator(), NULL, IID_PPV_ARGS(&m_pCommandList));


	D3D12_ROOT_DESCRIPTOR wvpRootDescriptor;
	wvpRootDescriptor.RegisterSpace = 0;
	wvpRootDescriptor.ShaderRegister = 0;

	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = wvpRootDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters),
		rootParameters,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr), S_OK);

	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);



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

	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc;
	//get the swapchaindesc to get its sample desc into the pso
	D3DClass::GetSwapChain()->GetDesc(&tempSwapChainDesc);

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
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// create the pso
	_pPSO = pPsoHandler->CreatePipelineStateObject(&psoDesc, D3DClass::GetDevice());

	//release after assigned to the pso?
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);

	//Create a descriptor heap for depthstencil
	D3D12_DESCRIPTOR_HEAP_DESC depthStencilStateDescriptorHeadpDesc = {};
	depthStencilStateDescriptorHeadpDesc.NumDescriptors = 1;
	depthStencilStateDescriptorHeadpDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	depthStencilStateDescriptorHeadpDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&depthStencilStateDescriptorHeadpDesc, IID_PPV_ARGS(&m_pDepthStencilDescriptorHeap)), S_OK);


	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = depthStencilViewDesc.Format;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;



	D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WindowClass::GetWidth(), WindowClass::GetHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_pDepthStencilBuffer)
		);
	m_pDepthStencilDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

	D3DClass::GetDevice()->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	Vertex boxVertexList[] = {
		// front face
		{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

		// right side face
		{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

		// left side face
		{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

		// back face
		{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

		// top face
		{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f }, 
		{ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

		// bottom face
		{ 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
	};
	int iVertexBufferSize = sizeof(boxVertexList);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pVertexBuffer)), S_OK);
	m_pVertexBuffer->SetName(L"BOX - VERTEX BUFFER RESOURECE HEAP");

	ID3D12Resource* pVertexBufferUpploadHeap;
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pVertexBufferUpploadHeap)), S_OK);
	pVertexBufferUpploadHeap->SetName(L"BOX - VERTEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA vertexInitData = {};
	vertexInitData.pData = reinterpret_cast<BYTE*>(boxVertexList);
	vertexInitData.RowPitch = iVertexBufferSize;
	vertexInitData.SlicePitch = iVertexBufferSize;

	UpdateSubresources(m_pCommandList, m_pVertexBuffer, pVertexBufferUpploadHeap, 0, 0, 1, &vertexInitData);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	DWORD boxIndexBufferList[] = {
		// ffront face
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle

				 // left face
		4, 5, 6, // first triangle
		4, 7, 5, // second triangle

				 // right face
		8, 9, 10, // first triangle
		8, 11, 9, // second triangle

				  // back face
		12, 13, 14, // first triangle
		12, 15, 13, // second triangle

					// top face
		16, 17, 18, // first triangle
		16, 19, 17, // second triangle

					// bottom face
		20, 21, 22, // first triangle
		20, 23, 21, // second triangle
	};
	int iIndexBufferSize = sizeof(boxIndexBufferList);

	m_iNumCubeIndices = sizeof(boxIndexBufferList) / sizeof(DWORD);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iIndexBufferSize), 
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, 
		nullptr, 
		IID_PPV_ARGS(&m_pIndexBuffer)), S_OK);

	m_pIndexBuffer->SetName(L"BOX- INDEXBUFFER RESOURCE HEAP");

	ID3D12Resource* pIndexBufferUploadHeap;
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iIndexBufferSize), 
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(&pIndexBufferUploadHeap)), S_OK);

	pIndexBufferUploadHeap->SetName(L"BOX- INDEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA indexData{};
	indexData.pData = reinterpret_cast<BYTE*>(boxIndexBufferList);
	indexData.RowPitch = iIndexBufferSize;
	indexData.SlicePitch = iIndexBufferSize;

	UpdateSubresources(m_pCommandList, m_pIndexBuffer, pIndexBufferUploadHeap, 0, 0, 1, &indexData);

	m_pCommandList->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, 
			D3D12_RESOURCE_STATE_COPY_DEST, 
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	

	for (unsigned int i = 0; i < g_cFrameBufferCount; ++i)
	{
		DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pWVPMatUpploadHeaps[i])), S_OK);

		m_pWVPMatUpploadHeaps[i]->SetName(L"WVP uppload resource heap");

		ZeroMemory(&m_cbWVP, sizeof(m_cbWVP));

		CD3DX12_RANGE readRange(0, 0);

		DxAssert(m_pWVPMatUpploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pWVPGPUAdress[i])), S_OK);
		
		memcpy(m_pWVPGPUAdress[i], &m_cbWVP, sizeof(m_cbWVP));

	}

	DirectX::XMMATRIX tempTranslationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&pos));
	DirectX::XMMATRIX tempRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat4(&rot));

	DirectX::XMMATRIX tempWorldMatrix = DirectX::XMMatrixMultiply(tempRotationMatrix, tempTranslationMatrix);
	DirectX::XMStoreFloat4x4(&m_xmTranslationMatrix, tempTranslationMatrix);
	DirectX::XMStoreFloat4x4(&m_xmRotationMatrix, tempRotationMatrix);
	DirectX::XMStoreFloat4x4(&m_xmWorldMatrix, tempWorldMatrix);

	


	DxAssert(m_pCommandList->Close(), S_OK);

	

	D3DClass::QueueGraphicsCommandList(m_pCommandList);
	//D3DClass::IncrementFenceValue();
	D3DClass::ExecuteGraphicsCommandLists();


	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = iVertexBufferSize;

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = iIndexBufferSize;

	SAFE_RELEASE(pVertexBufferUpploadHeap);
	SAFE_RELEASE(pIndexBufferUploadHeap);
}

BoxObject::~BoxObject()
{
	SAFE_RELEASE(m_pRootSignature);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pDepthStencilBuffer);
	SAFE_RELEASE(m_pDepthStencilDescriptorHeap);
	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pWVPMatUpploadHeaps[i]);
		
		//delete m_pWVPGPUAdress[i];//...
	}
	
	SAFE_RELEASE(m_pCommandList);
}

void BoxObject::Update(Camera * cam)
{
	DirectX::XMMATRIX xmRotXMAt = DirectX::XMMatrixRotationX(0.0001f);
	DirectX::XMMATRIX xmRotYMAt = DirectX::XMMatrixRotationY(0.0002f);
	DirectX::XMMATRIX xmRotZMat = DirectX::XMMatrixRotationZ(0.0003f);

	DirectX::XMMATRIX xmConcatRotMat = DirectX::XMLoadFloat4x4(&m_xmRotationMatrix) * xmRotXMAt * xmRotYMAt * xmRotZMat;
	DirectX::XMStoreFloat4x4(&m_xmRotationMatrix, xmConcatRotMat);

	//DirectX::XMMATRIX dxTranslationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(asdfr));

	DirectX::XMMATRIX xmTempWorldMat = xmConcatRotMat * DirectX::XMLoadFloat4x4(&m_xmTranslationMatrix);
	DirectX::XMStoreFloat4x4(&m_xmWorldMatrix, xmTempWorldMat);

	DirectX::XMMATRIX xmTempWVPMat = xmTempWorldMat * cam->GetVPMatrix();
	DirectX::XMMATRIX xmTransposedTempWVPMat = DirectX::XMMatrixTranspose(xmTempWVPMat);

	DirectX::XMStoreFloat4x4(&m_cbWVP.wvpMat, xmTransposedTempWVPMat);

	memcpy(m_pWVPGPUAdress[D3DClass::GetFrameIndex()], &m_cbWVP, sizeof(m_cbWVP));


	
}

void BoxObject::Draw(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, Camera * camera)
{
	DxAssert(m_pCommandList->Reset(D3DClass::GetCurrentCommandAllocator(), _pPSO), S_OK);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(D3DClass::GetCurrentRenderTarget(), 
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptorHandle(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &depthStencilDescriptorHandle);

	m_pCommandList->ClearDepthStencilView(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


	const float clearColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);



	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	m_pCommandList->RSSetViewports(1, &camera->GetViewport());
	m_pCommandList->RSSetScissorRects(1, &camera->GetScissorRect());
	m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_pCommandList->IASetIndexBuffer(&m_indexBufferView);


	m_pCommandList->SetGraphicsRootConstantBufferView(0, m_pWVPMatUpploadHeaps[D3DClass::GetFrameIndex()]->GetGPUVirtualAddress());

	m_pCommandList->DrawIndexedInstanced(m_iNumCubeIndices, 1, 0, 0, 0);


	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(D3DClass::GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	DxAssert(m_pCommandList->Close(), S_OK);
}

ID3D12CommandList * BoxObject::GetGraphicsCommandList()
{
	return m_pCommandList;
}
