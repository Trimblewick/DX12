#include "GrassBlades.h"


GrassBlades::GrassBlades()
{
	HRESULT hr;
	D3D12_SHADER_BYTECODE vsByteCode = {};
	D3D12_SHADER_BYTECODE gsByteCode = {};
	D3D12_SHADER_BYTECODE psByteCode = {};
	ID3DBlob* pVSblob = nullptr;
	ID3DBlob* pGSblob = nullptr;
	ID3DBlob* pPSblob = nullptr;
	ID3DBlob* pEblob = nullptr;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_ROOT_DESCRIPTOR wvpRootDescriptor;

	D3D12_DESCRIPTOR_RANGE heightmapDescriptorRanges[1];
	D3D12_STATIC_SAMPLER_DESC heightmapSampler = {};

	CD3DX12_ROOT_PARAMETER rootParameters[4];

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	D3D12_SHADER_RESOURCE_VIEW_DESC		SRVdescHeightMap = {};

	int iVertexBufferSize;
	ID3D12Resource* pGrassBladesVertexBufferUploadHeap;

	ID3D12Resource* pHeightMapUpploadHeap;

	ID3D12Fence* pUploadBufferFence;
	HANDLE fenceHandle;
	DxAssert(D3DClass::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pUploadBufferFence)), S_OK);
	fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	DxAssert(D3DClass::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCA)), S_OK);
	DxAssert(D3DClass::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCA, nullptr, IID_PPV_ARGS(&m_pCL)), S_OK);

	//compile vertexshader
	hr = D3DCompileFromFile(
		L"GrassVS.hlsl",
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

	hr = D3DCompileFromFile(
		L"GrassGS.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pGSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	gsByteCode.BytecodeLength = pGSblob->GetBufferSize();
	gsByteCode.pShaderBytecode = pGSblob->GetBufferPointer();

	//compile pixelshader
	hr = D3DCompileFromFile(
		L"GrassPS.hlsl",
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

	heightmapDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	heightmapDescriptorRanges[0].NumDescriptors = 1;
	heightmapDescriptorRanges[0].BaseShaderRegister = 0;
	heightmapDescriptorRanges[0].RegisterSpace = 0;
	heightmapDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[0].InitAsConstantBufferView(0, 0);
	rootParameters[1].InitAsUnorderedAccessView(0, 0);
	rootParameters[2].InitAsConstants(4, 1, 0);
	rootParameters[3].InitAsDescriptorTable(_countof(heightmapDescriptorRanges), heightmapDescriptorRanges);
	

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
	heightmapSampler.ShaderRegister = 0;
	heightmapSampler.RegisterSpace = 0;
	heightmapSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	rootSignatureDesc.Init(_countof(rootParameters),
		rootParameters,
		1,
		&heightmapSampler,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr), S_OK);
	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);


	//fetch the swapchain desc
	DXGI_SWAP_CHAIN_DESC tempSwapChainDesc;
	D3DClass::GetSwapChain()->GetDesc(&tempSwapChainDesc);

	//ooh here we go, pso
	//psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = vsByteCode;
	psoDesc.GS = gsByteCode;
	psoDesc.PS = psByteCode;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = tempSwapChainDesc.SampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.NumRenderTargets = 1;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	DxAssert(D3DClass::GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPSO)), S_OK);

	std::srand(std::time(0));
	const int nrOfStraws = 16384;

	m_uiGrassInstances = nrOfStraws;
	
	StructGrass* patch =  new StructGrass[nrOfStraws];
	DirectX::XMFLOAT4 binorm;
	DirectX::XMFLOAT3 forward;
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0));
	float length;
	

	for (int i = 0; i < nrOfStraws; ++i)
	{
		binorm.x = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
		binorm.z = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
		binorm.y = 0.0f;
		binorm.w = 0.0f;
		length = std::sqrt(binorm.x*binorm.x + binorm.z*binorm.z);
		binorm.x /= length;
		binorm.z /= length;

		patch[i].binormal = binorm;

		DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Cross(DirectX::XMLoadFloat4(&binorm), up));

		patch[i].position[0].x = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
		patch[i].position[0].y = 0.0f;
		patch[i].position[0].z = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
		patch[i].position[0].w = 1.0f;
		for (int j = 1; j < 4; ++j)
		{
			patch[i].position[j].x = patch[i].position[j - 1].x + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 0.02f - 0.01f;
			patch[i].position[j].y = patch[i].position[j - 1].y + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 0.16f;
			patch[i].position[j].z = patch[i].position[j - 1].z + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX) * 0.02f - 0.01f;
			patch[i].position[j].w = 1.0f;
		}
		
		

		patch[i].seed.x = 0.015f;
		patch[i].seed.y = 0.01f;
		patch[i].seed.z = 0.008f;
		patch[i].seed.w = 0.0f;
	}

	iVertexBufferSize = sizeof(StructGrass) * m_uiGrassInstances;

	m_UAVdescGrassBlades = {};
	m_UAVdescGrassBlades.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	m_UAVdescGrassBlades.Format = DXGI_FORMAT_UNKNOWN;
	m_UAVdescGrassBlades.Buffer.CounterOffsetInBytes = 0;
	m_UAVdescGrassBlades.Buffer.FirstElement = 0;
	m_UAVdescGrassBlades.Buffer.Flags = D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_NONE;
	m_UAVdescGrassBlades.Buffer.NumElements = m_uiGrassInstances;
	m_UAVdescGrassBlades.Buffer.StructureByteStride = sizeof(StructGrass);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pBufferGrassBladesList)), S_OK);
	m_pBufferGrassBladesList->SetName(L"GRASS BLADE INSTANCES - VERTEX BUFFER RESOURECE HEAP");


	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pGrassBladesVertexBufferUploadHeap)), S_OK);
	pGrassBladesVertexBufferUploadHeap->SetName(L"GRASS BLADE INSTANCES - VERTEX BUFFER UPLOAD HEAP");



	D3D12_SUBRESOURCE_DATA vertexInitData = {};
	vertexInitData.pData = reinterpret_cast<BYTE*>(patch);
	vertexInitData.RowPitch = iVertexBufferSize;
	vertexInitData.SlicePitch = iVertexBufferSize;

	UpdateSubresources(m_pCL, m_pBufferGrassBladesList, pGrassBladesVertexBufferUploadHeap, 0, 0, 1, &vertexInitData);

	m_pCL->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pBufferGrassBladesList,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	Texture* pHeightMap = new Texture(L"../Resources/h.png");

	SRVdescHeightMap.Format = pHeightMap->GetTextureDesc()->Format;
	SRVdescHeightMap.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVdescHeightMap.Texture2D.MipLevels = 1;
	SRVdescHeightMap.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		pHeightMap->GetTextureDesc(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pTextureHeightMap)), S_OK);
	m_pTextureHeightMap->SetName(L"heightmap resource heap");

	UINT64 ui64HeightMapSize;
	D3DClass::GetDevice()->GetCopyableFootprints(pHeightMap->GetTextureDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &ui64HeightMapSize);


	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ui64HeightMapSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pHeightMapUpploadHeap)
	), S_OK);
	pHeightMapUpploadHeap->SetName(L"UPLOAD HEIGHTMAP - GRASSBLADES");

	D3D12_SUBRESOURCE_DATA heightMapInitData = {};
	heightMapInitData.pData = pHeightMap->GetTextureData();
	heightMapInitData.RowPitch = pHeightMap->GetBytersPerRow();
	heightMapInitData.SlicePitch = pHeightMap->GetBytersPerRow() * pHeightMap->GetTextureHeight();

	UpdateSubresources(
		m_pCL,
		m_pTextureHeightMap,
		pHeightMapUpploadHeap,
		0,
		0,
		1,
		&heightMapInitData);

	m_pCL->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pTextureHeightMap,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.NodeMask = 0;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pDHHeightMap)), S_OK);

	INT DHsize = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_pDHHeightMap->GetCPUDescriptorHandleForHeapStart());
	D3DClass::GetDevice()->CreateShaderResourceView(m_pTextureHeightMap, &SRVdescHeightMap, cpuHandle);// m_pDHHeightMap->GetCPUDescriptorHandleForHeapStart());
	cpuHandle.Offset(DHsize);

	m_pCL->SetGraphicsRootSignature(m_pRootSignature);
	m_pCL->SetGraphicsRootUnorderedAccessView(1, m_pBufferGrassBladesList->GetGPUVirtualAddress());

	DxAssert(m_pCL->Close(), S_OK);
	D3DClass::QueueGraphicsCommandList(m_pCL);
	D3DClass::ExecuteGraphicsCommandLists();
	D3DClass::GetCommandQueue()->Signal(pUploadBufferFence, 1);


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

		m_pWVPMatUpploadHeaps[i]->SetName(L"WVP(grassblades) uppload resource heap");

		ZeroMemory(&m_wvpMat, sizeof(m_wvpMat));

		CD3DX12_RANGE readRange(0, 0);

		DxAssert(m_pWVPMatUpploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pWVPGPUAdress[i])), S_OK);

		memcpy(m_pWVPGPUAdress[i], &m_wvpMat, sizeof(m_wvpMat));

	}

	

	DxAssert(pUploadBufferFence->SetEventOnCompletion(1, fenceHandle), S_OK);
	WaitForSingleObject(fenceHandle, INFINITE);

	SAFE_RELEASE(pGrassBladesVertexBufferUploadHeap);
	SAFE_RELEASE(pHeightMapUpploadHeap);
	SAFE_RELEASE(pUploadBufferFence);
	
	//OK WTF, DO SOMETHING WITH THIS??!
	//DISASTER
	SAFE_RELEASE(m_pCL);
	SAFE_RELEASE(m_pCA);
	if (patch)
	{
		delete[] patch;
		patch = nullptr;
	}
	if (pHeightMap)
	{
		delete pHeightMap;
		pHeightMap = nullptr;
	}
}

GrassBlades::~GrassBlades()
{

	SAFE_RELEASE(m_pBufferGrassBladesList);
	SAFE_RELEASE(m_pTextureHeightMap);
	SAFE_RELEASE(m_pDHHeightMap);
	SAFE_RELEASE(m_pRootSignature);
	SAFE_RELEASE(m_pPSO);

	for (int i = 0; i < g_cFrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pWVPMatUpploadHeaps[i]);
	}

}

void GrassBlades::Update(Camera * camera)
{
	DirectX::XMMATRIX transposedWVPMat = DirectX::XMMatrixTranspose(camera->GetVPMatrix());
	DirectX::XMStoreFloat4x4(&m_wvpMat.wvpMat, transposedWVPMat);
	memcpy(m_pWVPGPUAdress[D3DClass::GetFrameIndex()], &m_wvpMat, sizeof(m_wvpMat));
	return;
}

void GrassBlades::Draw(FrameBuffer* pFrameBuffer, Camera* camera)
{
	//DxAssert(m_pCL->Reset(m_pCA, m_pPSO), S_OK);

	/*D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle = D3DClass::GetRTVDescriptorHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE depthHande = pFrameBuffer->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	m_pCL->OMSetRenderTargets(1, rtvHandle, false, &depthHande);*/

	m_pCL = pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD);

	m_pCL->SetGraphicsRootSignature(m_pRootSignature);
	m_pCL->SetPipelineState(m_pPSO);

	m_pCL->RSSetViewports(1, &camera->GetViewport());
	m_pCL->RSSetScissorRects(1, &camera->GetScissorRect());

	m_pCL->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//m_pCL->IASetVertexBuffers(0, 1, &m_grassBladesListVertexBufferView);
	m_pCL->SetDescriptorHeaps(1, &m_pDHHeightMap);
	m_pCL->SetGraphicsRootConstantBufferView(0, m_pWVPMatUpploadHeaps[D3DClass::GetFrameIndex()]->GetGPUVirtualAddress());
	m_pCL->SetGraphicsRootUnorderedAccessView(1, m_pBufferGrassBladesList->GetGPUVirtualAddress());

	m_pCL->SetGraphicsRoot32BitConstant(2, 5, 0);
	m_pCL->SetGraphicsRoot32BitConstant(2, 5, 1);
	m_pCL->SetGraphicsRoot32BitConstant(2, 25, 2);
	m_pCL->SetGraphicsRoot32BitConstant(2, 512, 3);

	m_pCL->SetGraphicsRootDescriptorTable(3, m_pDHHeightMap->GetGPUDescriptorHandleForHeapStart());
	

	m_pCL->DrawInstanced(m_uiGrassInstances, 1, 0, 0);

	//DxAssert(m_pCL->Close(), S_OK);
	//D3DClass::QueueGraphicsCommandList(m_pCL);


	return;
}
