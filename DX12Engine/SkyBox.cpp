#include "SkyBox.h"


SkyBox::SkyBox()
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

	D3D12_ROOT_DESCRIPTOR wvpRootDescriptor;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

	CD3DX12_ROOT_PARAMETER rootParameters[2];
	D3D12_DESCRIPTOR_RANGE rangeCubeMap[1];

	D3D12_STATIC_SAMPLER_DESC samplerCubemap = {};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	ID3D12Resource* pUploadBufferCubeMap;

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVdescCubeMap = {};
	Texture* pTextureCubeMap;

	ID3D12Resource* pUploadBufferVertex;
	ID3D12Resource* pUploadBufferIndex;

	ID3D12Fence* pFenceInit;
	HANDLE fenceHandle;
	DxAssert(D3DClass::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFenceInit)), S_OK);
	fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	ID3D12CommandAllocator* pCA;
	ID3D12GraphicsCommandList* pCL;
	DxAssert(D3DClass::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCA)), S_OK);
	DxAssert(D3DClass::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCA, nullptr, IID_PPV_ARGS(&pCL)), S_OK);

	//compile vertexshader
	hr = D3DCompileFromFile(
		L"SkyBoxVS.hlsl",
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
		L"SkyBoxPS.hlsl",
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

	//fill the shaderbytecode
	psByteCode.BytecodeLength = pPSblob->GetBufferSize();
	psByteCode.pShaderBytecode = pPSblob->GetBufferPointer();

	wvpRootDescriptor.ShaderRegister = 0;
	wvpRootDescriptor.RegisterSpace = 0;

	rangeCubeMap[0].BaseShaderRegister = 0;
	rangeCubeMap[0].NumDescriptors = 1;
	rangeCubeMap[0].RegisterSpace = 0;
	rangeCubeMap[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	rangeCubeMap[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].InitAsConstantBufferView(0, 0);
	rootParameters[1].InitAsDescriptorTable(1, rangeCubeMap, D3D12_SHADER_VISIBILITY_PIXEL);
	

	samplerCubemap.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerCubemap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerCubemap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerCubemap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerCubemap.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerCubemap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerCubemap.MaxAnisotropy = 1;
	samplerCubemap.MaxLOD = D3D12_FLOAT32_MAX;
	samplerCubemap.MinLOD = 0.0f;
	samplerCubemap.MipLODBias = 0;
	samplerCubemap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootSignatureDesc.Init(_countof(rootParameters),
		rootParameters,
		1,
		&samplerCubemap,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	
	ID3DBlob* sigblob;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigblob, nullptr), S_OK);
	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, sigblob->GetBufferPointer(), sigblob->GetBufferSize(), IID_PPV_ARGS(&m_pRS)), S_OK);

	DXGI_SWAP_CHAIN_DESC swapchaindescHolder;
	D3DClass::GetSwapChain()->GetDesc(&swapchaindescHolder);

	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAGS::D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.NodeMask = 0;
	psoDesc.pRootSignature = m_pRS;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.PS = psByteCode;
	psoDesc.VS = vsByteCode;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = swapchaindescHolder.SampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	DxAssert(D3DClass::GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPSO)), S_OK);

	

	//skybox_texture.jpg     //skybox_clouds.png		//Skybox_example.png		//skybox_braynz.png
	pTextureCubeMap = new Texture(L"../Resources/Skybox_example.png");

	SRVdescCubeMap.TextureCube.MipLevels = -1;
	SRVdescCubeMap.TextureCube.MostDetailedMip = 0;
	SRVdescCubeMap.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVdescCubeMap.Format = pTextureCubeMap->GetTextureDesc()->Format;
	SRVdescCubeMap.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		pTextureCubeMap->GetTextureDesc(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pBufferTexture)), S_OK);
	m_pBufferTexture->SetName(L"heightmap resource heap");

	UINT64 ui64HeightMapSize;
	D3DClass::GetDevice()->GetCopyableFootprints(pTextureCubeMap->GetTextureDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &ui64HeightMapSize);


	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ui64HeightMapSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pUploadBufferCubeMap)
	), S_OK);
	pUploadBufferCubeMap->SetName(L"UPLOAD HEIGHTMAP - GRASSBLADES");

	D3D12_SUBRESOURCE_DATA cubeMapInitData = {};
	cubeMapInitData.pData = pTextureCubeMap->GetTextureData();
	cubeMapInitData.RowPitch = pTextureCubeMap->GetBytersPerRow();

	UpdateSubresources(
		pCL,
		m_pBufferTexture, 
		pUploadBufferCubeMap, 
		0, 
		0, 
		1, 
		&cubeMapInitData);

	pCL->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pBufferTexture,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.NodeMask = 0;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pDH)), S_OK);

	m_iDHsize = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_pDH->GetCPUDescriptorHandleForHeapStart());
	D3DClass::GetDevice()->CreateShaderResourceView(m_pBufferTexture, &SRVdescCubeMap, cpuHandle);

	Vertex vertsCube[24];

	//front
	vertsCube[0] = { -1.0f, -1.0f, -1.0f, 1.0f, 0.666666666666666f };
	vertsCube[1] = { -1.0f, 1.0f, -1.0f, 1.0f, 0.333333333333333333f };
	vertsCube[2] = { 1.0f, 1.0f, -1.0f, 0.75f, 0.333333333333333333f };
	vertsCube[3] = { 1.0f, -1.0f, -1.0f, 0.75f, 0.666666666666666f };

	//back
	vertsCube[4] = { -1.0f, -1.0f, 1.0f, 0.25f, 0.666666666666666f };
	vertsCube[5] = { 1.0f, -1.0f, 1.0f, 0.5f, 0.666666666666666f };
	vertsCube[6] = { 1.0f, 1.0f, 1.0f, 0.5f, 0.333333333333333333f };
	vertsCube[7] = { -1.0f, 1.0f, 1.0f, 0.25f, 0.333333333333333333f };
	
	//top
	vertsCube[8] = { -1.0f, 1.0f, 1.0f, 0.25f, 0.333333333333333333f };
	vertsCube[9] = { 1.0f, 1.0f, 1.0f, 0.5f, 0.333333333333333333f };
	vertsCube[10] = { 1.0f, 1.0f, -1.0f, 0.5f, 0.0f };
	vertsCube[11] = { -1.0f, 1.0f, -1.0f, 0.25f, 0.0f };

	//bottom
	vertsCube[12] = { -1.0f, -1.0f, -1.0f, 0.25f, 1.0f };
	vertsCube[13] = { 1.0f, -1.0f, -1.0f, 0.5f, 1.0f };
	vertsCube[14] = { 1.0f, -1.0f, 1.0f, 0.5f, 0.666666666666666f };
	vertsCube[15] = { -1.0f, -1.0f, 1.0f, 0.25f, 0.666666666666666f };
	
	//left
	vertsCube[16] = { -1.0f, -1.0f, 1.0f, 0.25f, 0.666666666666666f };
	vertsCube[17] = { -1.0f, 1.0f, 1.0f, 0.25f, 0.333333333333333333f };
	vertsCube[18] = { -1.0f, 1.0f, -1.0f, 0.0f, 0.333333333333333333f };
	vertsCube[19] = { -1.0f, -1.0f, -1.0f, 0.0f, 0.666666666666666f };

	//right
	vertsCube[20] = { 1.0f, -1.0f, -1.0f, 0.75f, 0.666666666666666f };
	vertsCube[21] = { 1.0f, 1.0f, -1.0f, 0.75f, 0.333333333333333333f };
	vertsCube[22] = { 1.0f, 1.0f, 1.0f, 0.5f, 0.333333333333333333f };
	vertsCube[23] = { 1.0f, -1.0f, 1.0f, 0.5f, 0.666666666666666f };
	

	int iSizeVertexBuffer = sizeof(vertsCube);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iSizeVertexBuffer),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pBufferVetex)), S_OK);
	m_pBufferVetex->SetName(L"SKYBOX - VERTEX BUFFER RESOURECE HEAP");

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iSizeVertexBuffer),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pUploadBufferVertex)), S_OK);
	pUploadBufferVertex->SetName(L"PLANE - VERTEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA vertexInitData = {};
	vertexInitData.pData = reinterpret_cast<BYTE*>(vertsCube);
	vertexInitData.RowPitch = iSizeVertexBuffer;
	vertexInitData.SlicePitch = iSizeVertexBuffer;

	UpdateSubresources(
		pCL, 
		m_pBufferVetex, 
		pUploadBufferVertex, 
		0, 
		0, 
		1, 
		&vertexInitData);

	pCL->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
		m_pBufferVetex, 
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	DWORD Indices[36] = 
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};
	int iSizeIndexBuffer = sizeof(Indices);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iSizeIndexBuffer),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pBufferIndex)
	), S_OK);
	
	
	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iSizeIndexBuffer),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pUploadBufferIndex)
	), S_OK);

	D3D12_SUBRESOURCE_DATA indexData{};
	indexData.pData = reinterpret_cast<BYTE*>(Indices);
	indexData.RowPitch = iSizeIndexBuffer;
	indexData.SlicePitch = iSizeIndexBuffer;

	UpdateSubresources(
		pCL,
		m_pBufferIndex,
		pUploadBufferIndex,
		0,
		0,
		1,
		&indexData
	);

	pCL->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pBufferIndex,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	m_viewVertex.BufferLocation = m_pBufferVetex->GetGPUVirtualAddress();
	m_viewVertex.SizeInBytes = iSizeVertexBuffer;
	m_viewVertex.StrideInBytes = sizeof(Vertex);

	m_viewIndex.BufferLocation = m_pBufferIndex->GetGPUVirtualAddress();
	m_viewIndex.Format = DXGI_FORMAT_R32_UINT;
	m_viewIndex.SizeInBytes = iSizeIndexBuffer;
	
	DxAssert(pCL->Close(), S_OK);

	D3DClass::QueueGraphicsCommandList(pCL);
	D3DClass::ExecuteGraphicsCommandLists();
	D3DClass::GetCommandQueue()->Signal(pFenceInit, 1);


	//create matrix buffer
	for (unsigned int i = 0; i < g_iBackBufferCount; ++i)
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



	DxAssert(pFenceInit->SetEventOnCompletion(1, fenceHandle), S_OK);
	WaitForSingleObject(fenceHandle, INFINITE);

	SAFE_RELEASE(pCL);
	SAFE_RELEASE(pCA);
	SAFE_RELEASE(pFenceInit);
	SAFE_RELEASE(pUploadBufferVertex);
	SAFE_RELEASE(pUploadBufferIndex);
	SAFE_RELEASE(pUploadBufferCubeMap);
	
	if (pTextureCubeMap)
	{ 
		delete pTextureCubeMap;
	}
	
	return;
}

SkyBox::~SkyBox()
{
	SAFE_RELEASE(m_pBufferVetex);
	SAFE_RELEASE(m_pBufferIndex);
	SAFE_RELEASE(m_pBufferTexture);
	SAFE_RELEASE(m_pDH);
	SAFE_RELEASE(m_pPSO);
	SAFE_RELEASE(m_pRS);
	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		SAFE_RELEASE(m_pWVPMatUpploadHeaps[i]);
	}
	
}

void SkyBox::CreateSphere(int iLatLines, int iLongLines)
{
	return;
}


void SkyBox::Update(Camera * camera)
{
	DirectX::XMMATRIX wMat = DirectX::XMMatrixMultiply(
		DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f),
		DirectX::XMMatrixTranslation(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z));
	
	DirectX::XMStoreFloat4x4(&m_wvpMat.worldMatrix, camera->GetViewMatrix());

	DirectX::XMMATRIX transposedWVPMat = DirectX::XMMatrixMultiplyTranspose(wMat, camera->GetVPMatrix());
	DirectX::XMStoreFloat4x4(&m_wvpMat.wvpMat, transposedWVPMat);
	memcpy(m_pWVPGPUAdress[D3DClass::GetFrameIndex()], &m_wvpMat, sizeof(m_wvpMat));

}


void SkyBox::Draw(ID3D12GraphicsCommandList* pCL, Camera* camera)
{
	pCL->SetGraphicsRootSignature(m_pRS);
	pCL->SetPipelineState(m_pPSO);
	pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pCL->IASetVertexBuffers(0, 1, &m_viewVertex);
	pCL->IASetIndexBuffer(&m_viewIndex);

	pCL->RSSetViewports(1, &camera->GetViewport());
	pCL->RSSetScissorRects(1, &camera->GetScissorRect());

	pCL->SetDescriptorHeaps(1, &m_pDH);
	pCL->SetGraphicsRootConstantBufferView(0, m_pWVPMatUpploadHeaps[D3DClass::GetFrameIndex()]->GetGPUVirtualAddress());
	pCL->SetGraphicsRootDescriptorTable(1, m_pDH->GetGPUDescriptorHandleForHeapStart());

	pCL->DrawIndexedInstanced(36, 1, 0, 0, 0);

	return;
}
