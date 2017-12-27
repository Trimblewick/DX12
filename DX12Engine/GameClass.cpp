#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

struct Tri
{
	DirectX::XMFLOAT4 p1;
	DirectX::XMFLOAT4 p2;
	DirectX::XMFLOAT4 p3;
	DirectX::XMFLOAT4 color;
};

Tri* GetBoxMesh()
{
	Tri* pMesh = new Tri[36];

	//Front
	pMesh[0].p1.x = 0.0f;
	pMesh[0].p1.y = 0.0f;
	pMesh[0].p1.z = 0.0f;
	pMesh[0].p1.w = 1.0f;

	pMesh[0].p2.x = 0.0f;
	pMesh[0].p2.y = 1.0f;
	pMesh[0].p2.z = 0.0f;
	pMesh[0].p2.w = 1.0f;

	pMesh[0].p3.x = 1.0f;
	pMesh[0].p3.y = 0.0f;
	pMesh[0].p3.z = 0.0f;
	pMesh[0].p3.w = 1.0f;

	pMesh[0].color.x = 0.0f;
	pMesh[0].color.y = 1.0f;
	pMesh[0].color.z = 0.0f;
	pMesh[0].color.w = 1.0f;

	pMesh[1].p1.x = 0.0f;
	pMesh[1].p1.y = 1.0f;
	pMesh[1].p1.z = 0.0f;
	pMesh[1].p1.w = 1.0f;

	pMesh[1].p2.x = 1.0f;
	pMesh[1].p2.y = 1.0f;
	pMesh[1].p2.z = 0.0f;
	pMesh[1].p2.w = 1.0f;

	pMesh[1].p3.x = 1.0f;
	pMesh[1].p3.y = 0.0f;
	pMesh[1].p3.z = 0.0f;
	pMesh[1].p3.w = 1.0f;

	pMesh[1].color.x = 0.0f;
	pMesh[1].color.y = 1.0f;
	pMesh[1].color.z = 0.0f;
	pMesh[1].color.w = 1.0f;

	//Back
	pMesh[2].p1.x = 1.0f;
	pMesh[2].p1.y = 0.0f;
	pMesh[2].p1.z = -1.0f;
	pMesh[2].p1.w = 1.0f;

	pMesh[2].p2.x = 1.0f;
	pMesh[2].p2.y = 1.0f;
	pMesh[2].p2.z = -1.0f;
	pMesh[2].p2.w = 1.0f;

	pMesh[2].p3.x = 0.0f;
	pMesh[2].p3.y = 0.0f;
	pMesh[2].p3.z = -1.0f;
	pMesh[2].p3.w = 1.0f;

	pMesh[2].color.x = 0.0f;
	pMesh[2].color.y = 1.0f;
	pMesh[2].color.z = 0.0f;
	pMesh[2].color.w = 1.0f;

	pMesh[3].p1.x = 1.0f;
	pMesh[3].p1.y = 1.0f;
	pMesh[3].p1.z = -1.0f;
	pMesh[3].p1.w = 1.0f;

	pMesh[3].p2.x = 0.0f;
	pMesh[3].p2.y = 1.0f;
	pMesh[3].p2.z = -1.0f;
	pMesh[3].p2.w = 1.0f;

	pMesh[3].p3.x = 0.0f;
	pMesh[3].p3.y = 0.0f;
	pMesh[3].p3.z = -1.0f;
	pMesh[3].p3.w = 1.0f;

	pMesh[3].color.x = 0.0f;
	pMesh[3].color.y = 1.0f;
	pMesh[3].color.z = 0.0f;
	pMesh[3].color.w = 1.0f;

	//Right
	pMesh[4].p1.x = 1.0f;
	pMesh[4].p1.y = 0.0f;
	pMesh[4].p1.z = -1.0f;
	pMesh[4].p1.w = 1.0f;

	pMesh[4].p2.x = 1.0f;
	pMesh[4].p2.y = 0.0f;
	pMesh[4].p2.z = 0.0f;
	pMesh[4].p2.w = 1.0f;

	pMesh[4].p3.x = 1.0f;
	pMesh[4].p3.y = 1.0f;
	pMesh[4].p3.z = 0.0f;
	pMesh[4].p3.w = 1.0f;

	pMesh[4].color.x = 0.0f;
	pMesh[4].color.y = 1.0f;
	pMesh[4].color.z = 1.0f;
	pMesh[4].color.w = 1.0f;

	pMesh[5].p1.x = 1.0f;
	pMesh[5].p1.y = 0.0f;
	pMesh[5].p1.z = -1.0f;
	pMesh[5].p1.w = 1.0f;

	pMesh[5].p2.x = 1.0f;
	pMesh[5].p2.y = 1.0f;
	pMesh[5].p2.z = -1.0f;
	pMesh[5].p2.w = 1.0f;

	pMesh[5].p3.x = 1.0f;
	pMesh[5].p3.y = 1.0f;
	pMesh[5].p3.z = 0.0f;
	pMesh[5].p3.w = 1.0f;

	pMesh[5].color.x = 0.0f;
	pMesh[5].color.y = 1.0f;
	pMesh[5].color.z = 1.0f;
	pMesh[5].color.w = 1.0f;

	return pMesh;
}

Tri* GenerateTriangles(int iNrOfTriangles)
{
	Tri* triangles = new Tri[iNrOfTriangles];
	for (int i = 0; i < iNrOfTriangles; ++i)
	{
		Tri temp;

		temp.p1.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.0f;
		temp.p2.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.p3.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f;
		temp.p1.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p2.y = 1.0f;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p3.y = 0.0f;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p1.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 2.0f;
		temp.p2.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 2.0f;
		temp.p3.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 2.0f;

		temp.p1.w = 1.0f;// static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p2.w = 1.0f;// static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p3.w = 1.0f;// static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.color.x = 0;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.color.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.color.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.color.w = 1.0f;
		
		triangles[i] = temp;
	}
	return triangles;
}

bool GameClass::Initialize()
{
	m_pMainCamera = new Camera(DirectX::XMFLOAT3(-15.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pRenderer = new DeferredRenderer();
	m_pGPUbridge = new GPUbridge();
	m_pResourceLoader = new ResourceLoader();


	m_pRenderer->Initialize(m_pGPUbridge->GetCQ());

	//LOAD MESH
	//m_pObject = new Object();
	//m_pObject->SetMesh(m_pResourceLoader->LoadMeshFromFile("../Resources/Teapot/teapot_n_glass.obj", Mesh::MeshLayout::VERTEXNORMAL, m_pGPUbridge));

	m_pDHlab = D3DClass::CreateDH(4, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	int iOffsetSize = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	
	desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	


	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		m_ppUAVTargets[i] = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, WindowClass::GetWidth(), WindowClass::GetHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS), D3D12_RESOURCE_STATE_GENERIC_READ);

		D3DClass::GetDevice()->CreateUnorderedAccessView(m_ppUAVTargets[i], nullptr, nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * i));
		//D3DClass::GetDevice()->CreateShaderResourceView(m_ppUAVTargets[i], nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * i));
	}

	const int iNrOfTriangles = 6;
	Tri* pTiranglesData = GetBoxMesh();//GenerateTriangles(iNrOfTriangles);

	ID3D12CommandAllocator* pCA = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ID3D12GraphicsCommandList* pCL = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, pCA);

	ID3D12Resource* pTriMatUploadHeap = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_UPLOAD, sizeof(Tri) * iNrOfTriangles, D3D12_RESOURCE_STATE_GENERIC_READ, L"TriMatrixUploadHeap");
	m_pTriangleMatrices = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, sizeof(Tri) * iNrOfTriangles, D3D12_RESOURCE_STATE_COPY_DEST, L"TriMatrixHeap");
	
	D3D12_SUBRESOURCE_DATA initData = {};
	initData.pData = reinterpret_cast<BYTE*>(pTiranglesData);
	initData.SlicePitch = sizeof(Tri) * iNrOfTriangles;
	initData.RowPitch = sizeof(Tri) * iNrOfTriangles;

	UpdateSubresources(pCL, m_pTriangleMatrices, pTriMatUploadHeap, 0, 0, 1, &initData);

	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER().Transition(m_pTriangleMatrices, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	ID3D12Fence* uploadFence = D3DClass::CreateFence();
	HANDLE uploadFenceHandle = CreateEvent(NULL, NULL, NULL, NULL);
	pCL->Close();
	ID3D12CommandList* ppCL[] = { pCL };

	m_pGPUbridge->ExecuteDecoupledCLs(1, ppCL, uploadFence, 1);
	uploadFence->SetEventOnCompletion(1, uploadFenceHandle);
	WaitForSingleObject(uploadFenceHandle, INFINITE);

	free(pTiranglesData);
	SAFE_RELEASE(pCL);
	SAFE_RELEASE(pCA);
	SAFE_RELEASE(pTriMatUploadHeap);

	D3D12_BUFFER_SRV buf = {};
	buf.NumElements = iNrOfTriangles;
	buf.StructureByteStride = sizeof(Tri);
	buf.FirstElement = 0;
	buf.Flags = D3D12_BUFFER_SRV_FLAG_NONE;


	D3D12_SHADER_RESOURCE_VIEW_DESC triangleMatrixDesc;
	triangleMatrixDesc.Buffer = buf;
	triangleMatrixDesc.Format = DXGI_FORMAT_UNKNOWN;
	triangleMatrixDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	triangleMatrixDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	D3DClass::GetDevice()->CreateShaderResourceView(m_pTriangleMatrices, &triangleMatrixDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * 3));//HOLY SH*T!! THIS IS HARD CODED

	m_pUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;
	m_pUAVDescriptorRanges[0].NumDescriptors = 3;
	m_pUAVDescriptorRanges[0].BaseShaderRegister = 0;
	m_pUAVDescriptorRanges[0].RegisterSpace = 0;
	m_pUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

	m_UAVDescriptorTable.NumDescriptorRanges = 1;
	m_UAVDescriptorTable.pDescriptorRanges = m_pUAVDescriptorRanges;

	D3D12_DESCRIPTOR_RANGE matRange[1];
	matRange[0].BaseShaderRegister = 1;
	matRange[0].NumDescriptors = 1;
	matRange[0].OffsetInDescriptorsFromTableStart = 3;
	matRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	matRange[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE matTable;
	matTable.NumDescriptorRanges = 1;
	matTable.pDescriptorRanges = matRange;

	//CREATE SHADER
	Shader* pComputeShader = m_pResourceLoader->CreateComputeShader(L"RayTracerCS.hlsl");
	Shader* pQuadPipeline = m_pResourceLoader->CreateShader(L"QuadVS.hlsl", L"QuadPS.hlsl");
	
	
	D3D12_ROOT_PARAMETER cameraRootParameter;
	cameraRootParameter.Descriptor = CD3DX12_ROOT_DESCRIPTOR(0, 0);
	cameraRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	cameraRootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_PARAMETER rtvRP;
	rtvRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rtvRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rtvRP.DescriptorTable = m_UAVDescriptorTable;


	D3D12_ROOT_PARAMETER indexRP;
	indexRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	indexRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	indexRP.Constants = CD3DX12_ROOT_CONSTANTS(3, 1);

	D3D12_ROOT_DESCRIPTOR triDescriptor;
	triDescriptor.RegisterSpace = 0;
	triDescriptor.ShaderRegister = 1;


	D3D12_ROOT_PARAMETER triangleRP;
	//triangleRP.DescriptorTable = matTable;
	
	triangleRP.Descriptor = triDescriptor;
	triangleRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	triangleRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	pComputeShader->AddRootParameter(cameraRootParameter);
	pComputeShader->AddRootParameter(rtvRP);
	pComputeShader->AddRootParameter(indexRP);
	pComputeShader->AddRootParameter(triangleRP);

	D3D12_DESCRIPTOR_RANGE srvRange[1];
	srvRange[0].BaseShaderRegister = 0;
	srvRange[0].NumDescriptors = 3;
	srvRange[0].OffsetInDescriptorsFromTableStart = 0;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE srvTable;
	srvTable.NumDescriptorRanges = 1;
	srvTable.pDescriptorRanges = srvRange;

	D3D12_ROOT_PARAMETER srvRP;
	srvRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	srvRP.DescriptorTable = srvTable;
	srvRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pQuadPipeline->AddSampler(CD3DX12_STATIC_SAMPLER_DESC(0));
	pQuadPipeline->AddRootParameter(srvRP);

	m_pComputeRS = D3DClass::CreateRS(pComputeShader);
	m_pComputePSO = D3DClass::CreateComputePSO(pComputeShader, m_pComputeRS);

	m_pGraphicsRS = D3DClass::CreateRS(pQuadPipeline);
	m_pGraphicsPSO = D3DClass::CreateGraphicsPSO(pQuadPipeline, m_pGraphicsRS);

	delete pComputeShader;
	delete pQuadPipeline;

	return true;
}

void GameClass::Update(Input* input, float dt)
{
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	m_pMainCamera->Update(input, dt, iBackBufferIndex);

}


bool GameClass::Render()
{
	ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pDHlab };
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_pGPUbridge->WaitForPreviousFrame(iBackBufferIndex);
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	pCL->SetDescriptorHeaps(1, ppDescriptorHeaps);
	pCL->SetComputeRootSignature(m_pComputeRS);
	pCL->SetPipelineState(m_pComputePSO);
	pCL->SetGraphicsRootSignature(m_pGraphicsRS);

	m_pMainCamera->BindCameraBuffer(0, pCL, iBackBufferIndex);
	pCL->SetComputeRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDHlab->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV  * iBackBufferIndex));
	pCL->SetComputeRoot32BitConstant(2, iBackBufferIndex, 0);
	pCL->SetComputeRoot32BitConstant(2, WindowClass::GetHeight(), 1);
	pCL->SetComputeRoot32BitConstant(2, WindowClass::GetWidth(), 2);
	pCL->SetComputeRootShaderResourceView(3, m_pTriangleMatrices->GetGPUVirtualAddress());//CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDHlab->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV * 3));
	pCL->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDHlab->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV * iBackBufferIndex));
	
	//pCL->SetComputeRoot32BitConstant(2, D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), 1)

	pCL->Dispatch(WindowClass::GetWidth() / 32, WindowClass::GetHeight() / 32, 1);

	m_pRenderer->UnlockNextRTV(pCL);
	pCL->SetPipelineState(m_pGraphicsPSO);
	pCL->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	pCL->DrawInstanced(4, 1, 0, 0);

	m_pRenderer->TransitionCurrentRTVIntoPrecentState(pCL);
	
	m_pGPUbridge->QueueGraphicsCL(pCL);
	m_pGPUbridge->ExecuteGrapichsCLs();

	m_pRenderer->PrecentNextFrame();
	
	return true;
}

void GameClass::CleanUp()
{
	if (m_pMainCamera)
	{
		delete m_pMainCamera;
		m_pMainCamera = nullptr;
	}
	
	if (m_pGPUbridge)
	{
		delete m_pGPUbridge;
		m_pGPUbridge = nullptr;
	}
	
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
	if (m_pResourceLoader)
	{
		delete m_pResourceLoader;
		m_pResourceLoader = nullptr;
	}
}
