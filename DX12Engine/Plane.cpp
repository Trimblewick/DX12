#include "Plane.h"


Plane::Plane(FrameBuffer* pFrameBuffer)
{
	HRESULT hr;
	D3D12_SHADER_BYTECODE vsByteCode = {};
	D3D12_SHADER_BYTECODE psByteCode = {};
	D3D12_SHADER_BYTECODE hsByteCode = {};
	D3D12_SHADER_BYTECODE dsByteCode = {};
	D3D12_SHADER_BYTECODE gsByteCode = {};
	ID3DBlob* pVSblob = nullptr;
	ID3DBlob* pPSblob = nullptr;
	ID3DBlob* pHSblob = nullptr;
	ID3DBlob* pDSblob = nullptr;
	ID3DBlob* pGSblob = nullptr;
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


	D3D12_DESCRIPTOR_RANGE planeDescriptorRanges[1];
	D3D12_ROOT_DESCRIPTOR_TABLE planeDescriptorTable;
	D3D12_DESCRIPTOR_RANGE heightmapDescriptorRanges[1];
	D3D12_ROOT_DESCRIPTOR_TABLE heightmapDescriptorTable;

	D3D12_STATIC_SAMPLER_DESC textureSampler = {};
	D3D12_STATIC_SAMPLER_DESC heightmapSampler = {};

	CD3DX12_ROOT_PARAMETER rootparameters[4];
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;


	Texture*		pHeightMap;
	Texture*		pPlaneTexture;
	ID3D12Resource* pVertexBufferUploadHeap;
	ID3D12Resource* pTextureBufferUploadHeap;
	ID3D12Resource* pHeightMapBufferUploadHeap;
	D3D12_DESCRIPTOR_HEAP_DESC textureDescriptorHeapDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC grassTextureSRVDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC heightMapSRVDesc = {};

	ID3D12Fence* pUploadBufferFence;
	HANDLE fenceHandle;
	DxAssert(D3DClass::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pUploadBufferFence)), S_OK);
	fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);



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

	
	//compile hullshader
	hr = D3DCompileFromFile(
		L"PlaneHS.hlsl",
		nullptr,
		nullptr,
		"main",
		"hs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pHSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	hsByteCode.BytecodeLength = pHSblob->GetBufferSize();
	hsByteCode.pShaderBytecode = pHSblob->GetBufferPointer();

	//compile domainshader
	hr = D3DCompileFromFile(
		L"PlaneDS.hlsl",
		nullptr,
		nullptr,
		"main",
		"ds_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pDSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	dsByteCode.BytecodeLength = pDSblob->GetBufferSize();
	dsByteCode.pShaderBytecode = pDSblob->GetBufferPointer();

	//compile geometryshader
	hr = D3DCompileFromFile(
		L"PlaneGS.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pDSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	gsByteCode.BytecodeLength = pDSblob->GetBufferSize();
	gsByteCode.pShaderBytecode = pDSblob->GetBufferPointer();




	wvpRootDescriptor.RegisterSpace = 0;
	wvpRootDescriptor.ShaderRegister = 0;

	planeDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	planeDescriptorRanges[0].NumDescriptors = 1;
	planeDescriptorRanges[0].BaseShaderRegister = 0;
	planeDescriptorRanges[0].RegisterSpace = 0;
	planeDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	planeDescriptorTable.NumDescriptorRanges = _countof(planeDescriptorRanges);
	planeDescriptorTable.pDescriptorRanges = planeDescriptorRanges;

	heightmapDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	heightmapDescriptorRanges[0].NumDescriptors = 1;
	heightmapDescriptorRanges[0].BaseShaderRegister = 1;
	heightmapDescriptorRanges[0].RegisterSpace = 0;
	heightmapDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	heightmapDescriptorTable.NumDescriptorRanges = _countof(heightmapDescriptorRanges);
	heightmapDescriptorTable.pDescriptorRanges = heightmapDescriptorRanges;

	rootparameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootparameters[0].Descriptor = wvpRootDescriptor;
	rootparameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootparameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparameters[1].DescriptorTable = planeDescriptorTable;
	rootparameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootparameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparameters[2].DescriptorTable = heightmapDescriptorTable;
	rootparameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootparameters[3].InitAsConstants(3, 1, 0);// , D3D12_SHADER_VISIBILITY_HULL);// ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	//rootparameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
	

	// create a static sampler
	textureSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	textureSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	textureSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	textureSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	textureSampler.MipLODBias = 0;
	textureSampler.MaxAnisotropy = 1;
	textureSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	textureSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	textureSampler.MinLOD = 0.0f;
	textureSampler.MaxLOD = D3D12_FLOAT32_MAX;
	textureSampler.ShaderRegister = 0;
	textureSampler.RegisterSpace = 0;
	textureSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	heightmapSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	heightmapSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	heightmapSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	heightmapSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	heightmapSampler.MipLODBias = 0;
	heightmapSampler.MaxAnisotropy = 1;
	heightmapSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	heightmapSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	heightmapSampler.MinLOD = 0.0f;
	heightmapSampler.MaxLOD = D3D12_FLOAT32_MAX;
	heightmapSampler.ShaderRegister = 1;
	heightmapSampler.RegisterSpace = 0;
	heightmapSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	D3D12_STATIC_SAMPLER_DESC samplers[] = { textureSampler, heightmapSampler };

	rootSignatureDesc.Init(_countof(rootparameters),
		rootparameters,
		_countof(samplers),
		samplers,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ID3DBlob* sig;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, nullptr), S_OK);

	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);

	//fetch the swapchain desc
	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc;
	D3DClass::GetSwapChain()->GetDesc(&tempSwapChainDesc);

	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = vsByteCode;
	psoDesc.PS = psByteCode;
	psoDesc.HS = hsByteCode;
	psoDesc.DS = dsByteCode;
	psoDesc.GS = gsByteCode;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = tempSwapChainDesc.SampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.NumRenderTargets = 1;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	DxAssert(D3DClass::GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPSO)), S_OK);


	pHeightMap = new Texture(L"../Resources/h.png");
	BYTE* data = pHeightMap->GetTextureData();
	
	int height = pHeightMap->GetTextureHeight();
	int width = pHeightMap->GetTextureWidth();
	int size = height * width;
	PlaneVertex* planeVertices = new PlaneVertex[size];
	int heightMapIndex = 0;
	float u = 0.0f;
	float v = 0.0f;
	int dataIndex = 0;

	for (int x = 0; x < width; ++x)
	{
		u = (float)x * 100.0f / (float)width;
		for (int z = 0; z < height; ++z)
		{
			v = (float)z * 100.0f / (float)height;
			planeVertices[heightMapIndex] = PlaneVertex((float)x, /*data[dataIndex] * 0.1*/0.0f, (float)z, u, v);

			heightMapIndex++;
			dataIndex += 4;
		}
	}
	iVertexBufferSize = size * sizeof(PlaneVertex);
	

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
		IID_PPV_ARGS(&pVertexBufferUploadHeap)), S_OK);
	pVertexBufferUploadHeap->SetName(L"PLANE - VERTEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA vertexInitData = {};
	vertexInitData.pData = reinterpret_cast<BYTE*>(planeVertices);
	vertexInitData.RowPitch = iVertexBufferSize;
	vertexInitData.SlicePitch = iVertexBufferSize;

	UpdateSubresources(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), m_pVertexBuffer, pVertexBufferUploadHeap, 0, 0, 1, &vertexInitData);

	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pVertexBuffer, 
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


	m_uiNrOfIndices = (width - 1) * (height - 1) * 6;
	DWORD* indices = new DWORD[m_uiNrOfIndices];
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

	iIndexBufferSize = m_uiNrOfIndices * sizeof(DWORD);

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



	//grasstexture.png		dirt1024texture.jpg
	pPlaneTexture = new Texture(L"../Resources/dirt1024texture.jpg");

	D3D12_RESOURCE_DESC planeResourceDesc = *pPlaneTexture->GetTextureDesc();
	planeResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	planeResourceDesc.MipLevels = 10;

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&planeResourceDesc,//pPlaneTexture->GetTextureDesc(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pTextureBuffer)), S_OK);
	m_pTextureBuffer->SetName(L"planetexture resource heap");


	UINT64 uploadtextureBufferSize;
	D3DClass::GetDevice()->GetCopyableFootprints(&planeResourceDesc/*pPlaneTexture->GetTextureDesc()*/, 0, 1, 0, nullptr, nullptr, nullptr, &uploadtextureBufferSize);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadtextureBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pTextureBufferUploadHeap)), S_OK);
	pTextureBufferUploadHeap->SetName(L"planetexture upload heap");


	D3D12_SUBRESOURCE_DATA textureInitData = {};
	textureInitData.pData = pPlaneTexture->GetTextureData();
	textureInitData.RowPitch = pPlaneTexture->GetBytersPerRow();
	textureInitData.SlicePitch = pPlaneTexture->GetBytersPerRow() * pPlaneTexture->GetTextureHeight();

	UpdateSubresources(
		pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), 
		m_pTextureBuffer, 
		pTextureBufferUploadHeap,
		0, 
		0, 
		1, 
		&textureInitData);

	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pTextureBuffer, 
			D3D12_RESOURCE_STATE_COPY_DEST, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	pPlaneTexture->GenerateMipMaps(m_pTextureBuffer, pFrameBuffer);



	//upload heightmap
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		pHeightMap->GetTextureDesc(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pHeightBuffer)), S_OK);
	m_pHeightBuffer->SetName(L"heightmap resource heap");

	UINT64 ui64HeightMapBufferSize;
	D3DClass::GetDevice()->GetCopyableFootprints(pHeightMap->GetTextureDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &ui64HeightMapBufferSize);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ui64HeightMapBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pHeightMapBufferUploadHeap)
	), S_OK);

	D3D12_SUBRESOURCE_DATA heightMapInitData = {};
	heightMapInitData.pData = pHeightMap->GetTextureData();
	heightMapInitData.RowPitch = pHeightMap->GetBytersPerRow();
	heightMapInitData.SlicePitch = pHeightMap->GetBytersPerRow() * pHeightMap->GetTextureHeight();

	UpdateSubresources(
		pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), 
		m_pHeightBuffer, 
		pHeightMapBufferUploadHeap, 
		0, 
		0, 
		1, 
		&heightMapInitData);

	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pHeightBuffer, 
			D3D12_RESOURCE_STATE_COPY_DEST, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	textureDescriptorHeapDesc.NumDescriptors = 2;
	textureDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	textureDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&textureDescriptorHeapDesc, IID_PPV_ARGS(&m_pTextureDH)), S_OK);
	m_uiTextureDescriptorSize = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(textureDescriptorHeapDesc.Type);
	
	DxAssert(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->Close(), S_OK);

	D3DClass::QueueGraphicsCommandList(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD));
	D3DClass::ExecuteGraphicsCommandLists();
	D3DClass::GetCommandQueue()->Signal(pUploadBufferFence, 1);


	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = iVertexBufferSize;
	m_vertexBufferView.StrideInBytes = sizeof(PlaneVertex);

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = iIndexBufferSize;


	grassTextureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	grassTextureSRVDesc.Format = pPlaneTexture->GetTextureDesc()->Format;
	grassTextureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	grassTextureSRVDesc.Texture2D.MipLevels = -1;
	grassTextureSRVDesc.Texture2D.MostDetailedMip = 0;
	
	
	heightMapSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	heightMapSRVDesc.Format = pHeightMap->GetTextureDesc()->Format;
	heightMapSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	heightMapSRVDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_pTextureDH->GetCPUDescriptorHandleForHeapStart());
	

	D3DClass::GetDevice()->CreateShaderResourceView(m_pTextureBuffer, &grassTextureSRVDesc, cpuHandle);
	cpuHandle.Offset(m_uiTextureDescriptorSize);
	D3DClass::GetDevice()->CreateShaderResourceView(m_pHeightBuffer, &heightMapSRVDesc, cpuHandle);
	cpuHandle.Offset(m_uiTextureDescriptorSize);

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
	
	DxAssert(pUploadBufferFence->SetEventOnCompletion(1, fenceHandle), S_OK);
	
	WaitForSingleObject(fenceHandle, INFINITE);
	
	SAFE_RELEASE(pVertexBufferUploadHeap);
	SAFE_RELEASE(pIndexBufferUploadHeap);
	SAFE_RELEASE(pTextureBufferUploadHeap);
	SAFE_RELEASE(pHeightMapBufferUploadHeap);
	SAFE_RELEASE(pUploadBufferFence);
	if (pPlaneTexture)
	{
		delete pPlaneTexture;
		pPlaneTexture = nullptr;
	}
	if (pHeightMap)
	{
		delete pHeightMap;
	}

	delete[] planeVertices;
	delete[] indices;
}

Plane::~Plane()
{
	SAFE_RELEASE(m_pTextureBuffer);
	SAFE_RELEASE(m_pHeightBuffer);
	SAFE_RELEASE(m_pTextureDH);
	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pWVPMatUpploadHeaps[i]);
	}
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pPSO);
	SAFE_RELEASE(m_pRootSignature);
	
}



void Plane::Update(Camera * camera)
{
	
	DirectX::XMMATRIX transposedWVPMat = DirectX::XMMatrixTranspose(camera->GetVPMatrix());//DirectX::XMMatrixMultiplyTranspose(DirectX::XMMatrixTranslation(-20, -20, -20), camera->GetVPMatrix());
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
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_pTextureDH->GetGPUDescriptorHandleForHeapStart());
	pCL->SetGraphicsRootDescriptorTable(1, gpuHandle);
	gpuHandle.Offset(m_uiTextureDescriptorSize);
	pCL->SetGraphicsRootDescriptorTable(2, gpuHandle);
	gpuHandle.Offset(m_uiTextureDescriptorSize);

	pCL->SetGraphicsRoot32BitConstant(3, DWParam(camera->GetPosition().x).Uint, 0);
	pCL->SetGraphicsRoot32BitConstant(3, DWParam(camera->GetPosition().y).Uint, 1);
	pCL->SetGraphicsRoot32BitConstant(3, DWParam(camera->GetPosition().z).Uint, 2);

	pCL->RSSetViewports(1, &camera->GetViewport());
	pCL->RSSetScissorRects(1, &camera->GetScissorRect());

	

	pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);//D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCL->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	pCL->IASetIndexBuffer(&m_indexBufferView);
	
	pCL->SetGraphicsRootConstantBufferView(0, m_pWVPMatUpploadHeaps[D3DClass::GetFrameIndex()]->GetGPUVirtualAddress());

	pCL->DrawIndexedInstanced(m_uiNrOfIndices, 1, 0, 0, 0);

	return;
}



