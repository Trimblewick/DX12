#include "Plane.h"




Plane::Plane(FrameBuffer* pFrameBuffer)
{
	HRESULT hr;
	D3D12_SHADER_BYTECODE vsByteCode = {};
	D3D12_SHADER_BYTECODE psByteCode = {};
	ID3DBlob* pVSblob = nullptr;
	ID3DBlob* pPSblob = nullptr;
	ID3DBlob* pEblob = nullptr;

	D3D12_INPUT_ELEMENT_DESC inputLayoutElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	//fill input layout desc
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayoutElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayoutElementDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	int iVertexBufferSize;
	int iIndexBufferSize;

	D3D12_ROOT_DESCRIPTOR wvpRootDescriptor;
	D3D12_DESCRIPTOR_RANGE planeDescriptorRanges[1];//for texture
	D3D12_ROOT_DESCRIPTOR_TABLE planeDescriptorTable;

	D3D12_STATIC_SAMPLER_DESC sampler = {};

	D3D12_ROOT_PARAMETER rootparameters[2];
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

	ID3D12Resource* pVertexBufferUpploadHeap;
	ID3D12Resource* pGrassTextureBufferUpploadHeap;
	D3D12_DESCRIPTOR_HEAP_DESC textureDescriptorHeapDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC grassTextureSRVDesc = {};

	ID3D12Fence* pUploadBufferFence;
	HANDLE fenceHandle;

	DxAssert(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->Reset(D3DClass::GetCurrentCommandAllocator(), nullptr), S_OK);

	//compile vertexshader
	hr = D3DCompileFromFile(
		L"PlaneVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pVSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	//fill the shaderbytecode
	vsByteCode.BytecodeLength = pVSblob->GetBufferSize();
	vsByteCode.pShaderBytecode = pVSblob->GetBufferPointer();

	//compile pixelshader
	hr = D3DCompileFromFile(
		L"PlanePS.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pPSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	psByteCode.BytecodeLength = pPSblob->GetBufferSize();
	psByteCode.pShaderBytecode = pPSblob->GetBufferPointer();

	

	wvpRootDescriptor.RegisterSpace = 0;
	wvpRootDescriptor.ShaderRegister = 0;

	planeDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	planeDescriptorRanges[0].NumDescriptors = 1;
	planeDescriptorRanges[0].BaseShaderRegister = 0;
	planeDescriptorRanges[0].RegisterSpace = 0;
	planeDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	planeDescriptorTable.NumDescriptorRanges = _countof(planeDescriptorRanges);
	planeDescriptorTable.pDescriptorRanges = &planeDescriptorRanges[0];

	rootparameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootparameters[0].Descriptor = wvpRootDescriptor;
	rootparameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootparameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparameters[1].DescriptorTable = planeDescriptorTable;
	rootparameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// create a static sampler
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	
	rootSignatureDesc.Init(_countof(rootparameters),
		rootparameters,
		1,
		&sampler,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	ID3DBlob* sig;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, nullptr), S_OK);

	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);

	//fetch the swapchain desc
	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc;
	D3DClass::GetSwapChain()->GetDesc(&tempSwapChainDesc);

	//ooh here we go, pso
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = vsByteCode;
	psoDesc.PS = psByteCode;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = tempSwapChainDesc.SampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.NumRenderTargets = 1;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	DxAssert(D3DClass::GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPSO)), S_OK);

	SAFE_RELEASE(pVSblob);
	SAFE_RELEASE(pPSblob);

	Texture* heightMap = new Texture(L"../Resources/h.png");
	BYTE* data = heightMap->GetTextureData();
	
	int height = heightMap->GetTextureHeight();
	int width = heightMap->GetTextureWidth();
	int size = height * width;
	PlaneVertex* planeVertices = new PlaneVertex[size];
	int heightMapIndex = 0;
	float u = 0.0f;
	float v = 0.0f;
	int dataIndex = 0;

	for (int x = 0; x < width; ++x)
	{
		u = (float)x * 10.0f / (float)width;
		for (int z = 0; z < height; ++z)
		{
			v = (float)z * 10.0f / (float)height;
			planeVertices[heightMapIndex] = PlaneVertex((float)x * 0.1f, data[dataIndex] * 0.1, (float)z * 0.1f, u, v);

			heightMapIndex++;
			dataIndex += 4;
		}
	}
	iVertexBufferSize = size * sizeof(PlaneVertex);
	
	
	//create vertex buffer and its initdata
	/*PlaneVertex planeVertices[] = {
		{5, 0, 5, 2, 2},
		{5, 0, -5, 2, 0},
		{ -5, 0, 5, 0, 2 },
		{ -5, 0, -5, 0, 0 }
		//,
		//{ -5, 0, 5, 0, 2 },
		//{ 5, 0, -5, 2, 0 },
		//{ 5, 0, 5, 2, 2 }
		
		//{-5, 0, 5, 0, 2},
		//{5, 0, 5, 2, 2},
		//{-5, 0, -5, 0, 0}

	};
	iVertexBufferSize = sizeof(planeVertices);
	*/

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pVertexBuffer)), S_OK);
	m_pVertexBuffer->SetName(L"PLANE - VERTEX BUFFER RESOURECE HEAP");

	
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pVertexBufferUpploadHeap)), S_OK);
	pVertexBufferUpploadHeap->SetName(L"PLANE - VERTEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA vertexInitData = {};
	vertexInitData.pData = reinterpret_cast<BYTE*>(planeVertices);
	vertexInitData.RowPitch = iVertexBufferSize;
	vertexInitData.SlicePitch = iVertexBufferSize;

	UpdateSubresources(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), m_pVertexBuffer, pVertexBufferUpploadHeap, 0, 0, 1, &vertexInitData);

	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pVertexBuffer, 
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


	m_iNrOfIndices = (width - 1) * (height - 1) * 6;
	DWORD* indices = new DWORD[m_iNrOfIndices];
	int squareIndex = 0;
	for (int i = 1; i < (width - 1)*(height - 1) + 1; ++i)
	{
		if (i % width == 0)
		{
			i++;
		}
		indices[squareIndex + 1] = i - 1;
		indices[squareIndex] = i + width - 1;
		indices[squareIndex + 2] = i;
		indices[squareIndex + 4] = i + width - 1;
		indices[squareIndex + 3] = i + width;
		indices[squareIndex + 5] = i;
		squareIndex += 6;
	}

/*	DWORD indices[] = {
		0, 1, 2,
		2, 3, 1
	};*/
	iIndexBufferSize = m_iNrOfIndices * sizeof(DWORD);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iIndexBufferSize),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pIndexBuffer)), S_OK);

	m_pIndexBuffer->SetName(L"Plane- INDEXBUFFER RESOURCE HEAP");

	ID3D12Resource* pIndexBufferUploadHeap;
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iIndexBufferSize),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pIndexBufferUploadHeap)), S_OK);

	pIndexBufferUploadHeap->SetName(L"Plane- INDEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA indexData{};
	indexData.pData = reinterpret_cast<BYTE*>(indices);
	indexData.RowPitch = iIndexBufferSize;
	indexData.SlicePitch = iIndexBufferSize;

	UpdateSubresources(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), m_pIndexBuffer, pIndexBufferUploadHeap, 0, 0, 1, &indexData);

	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));



	m_pGrassTexture = new Texture(L"../Resources/grasstexture.png");

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		m_pGrassTexture->GetTextureDesc(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pGrassTextureBuffer)), S_OK);
	m_pGrassTextureBuffer->SetName(L"Grass texture buffer resource heap");

	UINT64 uploadBufferSize;
	D3DClass::GetDevice()->GetCopyableFootprints(m_pGrassTexture->GetTextureDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pGrassTextureBufferUpploadHeap)), S_OK);
	pGrassTextureBufferUpploadHeap->SetName(L"Grass Texture upload heap");

	D3D12_SUBRESOURCE_DATA textureInitData = {};
	textureInitData.pData = m_pGrassTexture->GetTextureData();
	textureInitData.RowPitch = m_pGrassTexture->GetBytersPerRow();
	textureInitData.SlicePitch = m_pGrassTexture->GetBytersPerRow() * m_pGrassTexture->GetTextureHeight();

	UpdateSubresources(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), m_pGrassTextureBuffer, pGrassTextureBufferUpploadHeap, 0, 0, 1, &textureInitData);
	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pGrassTextureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	textureDescriptorHeapDesc.NumDescriptors = 1;
	textureDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	textureDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&textureDescriptorHeapDesc, IID_PPV_ARGS(&m_pTextureDH)), S_OK);
	

	DxAssert(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->Close(), S_OK);

	D3DClass::QueueGraphicsCommandList(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD));
	D3DClass::ExecuteGraphicsCommandLists();


	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = iVertexBufferSize;
	m_vertexBufferView.StrideInBytes = sizeof(PlaneVertex);

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = iIndexBufferSize;

	grassTextureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	grassTextureSRVDesc.Format = m_pGrassTexture->GetTextureDesc()->Format;
	grassTextureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	grassTextureSRVDesc.Texture2D.MipLevels = 1;
	D3DClass::GetDevice()->CreateShaderResourceView(m_pGrassTextureBuffer, &grassTextureSRVDesc, m_pTextureDH->GetCPUDescriptorHandleForHeapStart());

	//create matrix buffer
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

		ZeroMemory(&m_wvpMat, sizeof(m_wvpMat));

		CD3DX12_RANGE readRange(0, 0);

		DxAssert(m_pWVPMatUpploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pWVPGPUAdress[i])), S_OK);

		memcpy(m_pWVPGPUAdress[i], &m_wvpMat, sizeof(m_wvpMat));

	}

	




	//release vertexbuffer upload heap
	DxAssert(D3DClass::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pUploadBufferFence)), S_OK);
	fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	DxAssert(pUploadBufferFence->Signal(1), S_OK);
	DxAssert(pUploadBufferFence->SetEventOnCompletion(1, fenceHandle), S_OK);
	
	WaitForSingleObject(fenceHandle, INFINITE);
	
	//SAFE_RELEASE(pVertexBufferUpploadHeap);
	//SAFE_RELEASE(pIndexBufferUploadHeap);
	//SAFE_RELEASE(pGrassTextureBufferUpploadHeap);
	SAFE_RELEASE(pUploadBufferFence);
	if (m_pGrassTexture)
	{
		delete m_pGrassTexture;
		m_pGrassTexture = nullptr;
	}
}

Plane::~Plane()
{
	SAFE_RELEASE(m_pGrassTextureBuffer);
	SAFE_RELEASE(m_pTextureDH);
	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pWVPMatUpploadHeaps[i]);
	}
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pPSO);
	SAFE_RELEASE(m_pRootSignature);
	if (m_pGrassTexture)
	{
		delete m_pGrassTexture;
		m_pGrassTexture = nullptr;
	}
}



void Plane::Update(Camera * camera)
{
	DirectX::XMMATRIX transposedWVPMat = DirectX::XMMatrixTranspose(camera->GetVPMatrix());
	DirectX::XMStoreFloat4x4(&m_wvpMat.wvpMat, transposedWVPMat);
	memcpy(m_pWVPGPUAdress[D3DClass::GetFrameIndex()], &m_wvpMat, sizeof(m_wvpMat));

}

void Plane::Draw(FrameBuffer * pFrameBuffer, Camera * camera)
{
	ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pTextureDH };
	ID3D12GraphicsCommandList* pCL = pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD);
	
	pCL->SetGraphicsRootSignature(m_pRootSignature);
	pCL->SetPipelineState(m_pPSO);

	pCL->SetDescriptorHeaps(_countof(ppDescriptorHeaps), ppDescriptorHeaps);
	pCL->SetGraphicsRootDescriptorTable(1, m_pTextureDH->GetGPUDescriptorHandleForHeapStart());
	
	pCL->RSSetViewports(1, &camera->GetViewport());
	pCL->RSSetScissorRects(1, &camera->GetScissorRect());

	pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCL->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	pCL->IASetIndexBuffer(&m_indexBufferView);
	
	pCL->SetGraphicsRootConstantBufferView(0, m_pWVPMatUpploadHeaps[D3DClass::GetFrameIndex()]->GetGPUVirtualAddress());

	pCL->DrawIndexedInstanced(m_iNrOfIndices, 1, 0, 0, 0);

	return;
}



