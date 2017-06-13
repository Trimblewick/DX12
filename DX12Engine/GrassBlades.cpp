#include "GrassBlades.h"


GrassBlades::GrassBlades(FrameBuffer* pFrameBuffer)
{
	HRESULT hr;
	D3D12_SHADER_BYTECODE vsByteCode = {};
	D3D12_SHADER_BYTECODE psByteCode = {};
	ID3DBlob* pVSblob = nullptr;
	ID3DBlob* pPSblob = nullptr;
	ID3DBlob* pEblob = nullptr;

	D3D12_INPUT_ELEMENT_DESC inputLayoutElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	//fill input layout desc
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayoutElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayoutElementDesc;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_ROOT_DESCRIPTOR wvpRootDescriptor;
	D3D12_ROOT_PARAMETER rootParameters[1];
	D3D12_ROOT_DESCRIPTOR_TABLE grassBladesDescriptorTable;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	int iVertexBufferSize;
	ID3D12Resource* pGrassBladesVertexBufferUploadHeap;

	ID3D12Fence* pUploadBufferFence;
	HANDLE fenceHandle;


	DxAssert(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->Reset(D3DClass::GetCurrentCommandAllocator(), nullptr), S_OK);

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

	wvpRootDescriptor.RegisterSpace = 0;
	wvpRootDescriptor.ShaderRegister = 0;

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = wvpRootDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	/*grassBladesDescriptorTable.NumDescriptorRanges = 

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = grassBladesDescriptorTable;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;*/

	rootSignatureDesc.Init(_countof(rootParameters),
		rootParameters,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr), S_OK);
	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)), S_OK);

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

	//11
	Vertex test[] = {
		{ 0, 2, 2 },
		{ 2, 2, 2 },
		{ 0, 4, 2 },
		{ 2, 4, 2 },
		{ 0, 6, 2 },
		{ 2, 6, 2 },
		{ 0, 6, 2 },
		{ 2, 4, 2 },
		{ 0, 4, 2 },
		{ 2, 2, 2 },
		{ 0, 2, 2 }
	};
	iVertexBufferSize = sizeof(test);

	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_pGrassBladesListVertexBuffer)), S_OK);
	m_pGrassBladesListVertexBuffer->SetName(L"GRASS BLADE INSTANCES - VERTEX BUFFER RESOURECE HEAP");


	DxAssert(D3DClass::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(iVertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pGrassBladesVertexBufferUploadHeap)), S_OK);
	pGrassBladesVertexBufferUploadHeap->SetName(L"GRASS BLADE INSTANCES - VERTEX BUFFER UPLOAD HEAP");

	D3D12_SUBRESOURCE_DATA vertexInitData = {};
	vertexInitData.pData = reinterpret_cast<BYTE*>(test);
	vertexInitData.RowPitch = iVertexBufferSize;
	vertexInitData.SlicePitch = iVertexBufferSize;

	UpdateSubresources(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD), m_pGrassBladesListVertexBuffer, pGrassBladesVertexBufferUploadHeap, 0, 0, 1, &vertexInitData);

	pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_pGrassBladesListVertexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


	DxAssert(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD)->Close(), S_OK);
	D3DClass::QueueGraphicsCommandList(pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD));
	D3DClass::ExecuteGraphicsCommandLists();

	m_grassBladesListVertexBufferView.BufferLocation = m_pGrassBladesListVertexBuffer->GetGPUVirtualAddress();
	m_grassBladesListVertexBufferView.SizeInBytes = iVertexBufferSize;
	m_grassBladesListVertexBufferView.StrideInBytes = sizeof(Vertex);

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

	DxAssert(D3DClass::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pUploadBufferFence)), S_OK);
	fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	DxAssert(pUploadBufferFence->Signal(1), S_OK);
	DxAssert(pUploadBufferFence->SetEventOnCompletion(1, fenceHandle), S_OK);
	WaitForSingleObject(fenceHandle, INFINITE);

	SAFE_RELEASE(pGrassBladesVertexBufferUploadHeap);
	SAFE_RELEASE(pUploadBufferFence);


}

GrassBlades::~GrassBlades()
{

	SAFE_RELEASE(m_pGrassBladesListVertexBuffer);
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

void GrassBlades::Draw(FrameBuffer * pFrameBuffer, Camera* camera)
{
	ID3D12GraphicsCommandList* pCL = pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD);

	pCL->SetGraphicsRootSignature(m_pRootSignature);
	pCL->SetPipelineState(m_pPSO);

	pCL->RSSetViewports(1, &camera->GetViewport());
	pCL->RSSetScissorRects(1, &camera->GetScissorRect());

	pCL->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pCL->IASetVertexBuffers(0, 1, &m_grassBladesListVertexBufferView);

	pCL->SetGraphicsRootConstantBufferView(0, m_pWVPMatUpploadHeaps[D3DClass::GetFrameIndex()]->GetGPUVirtualAddress());

	pCL->DrawInstanced(11, 1, 0, 0);


	return;
}
