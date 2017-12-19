#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
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

	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		m_ppUAVTargets[i] = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, WindowClass::GetWidth() * WindowClass::GetHeight() * sizeof(float) * 4, D3D12_RESOURCE_STATE_GENERIC_READ, L"uav");
	}
	m_pSurfaceDescriptorHeap = D3DClass::CreateDH(3, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

	m_pUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	m_pUAVDescriptorRanges[0].NumDescriptors = 3;
	m_pUAVDescriptorRanges[0].BaseShaderRegister = 0;
	m_pUAVDescriptorRanges[0].RegisterSpace = 0;
	m_pUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

	m_UAVDescriptorTable.NumDescriptorRanges = 1;
	m_UAVDescriptorTable.pDescriptorRanges = m_pUAVDescriptorRanges;

	D3D12_DESCRIPTOR_RANGE srvRange[1];
	srvRange[0].BaseShaderRegister = 0;
	srvRange[0].NumDescriptors = 3;
	srvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE srvTable;
	srvTable.NumDescriptorRanges = 1;
	srvTable.pDescriptorRanges = srvRange;

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

	D3D12_ROOT_PARAMETER srvRP;
	srvRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	srvRP.DescriptorTable = srvTable;
	srvRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_PARAMETER indexRP;
	indexRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	indexRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	indexRP.Constants = CD3DX12_ROOT_CONSTANTS(1, 1);

	pComputeShader->AddRootParameter(cameraRootParameter);
	pComputeShader->AddRootParameter(rtvRP);
	pComputeShader->AddRootParameter(indexRP);


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
	ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pSurfaceDescriptorHeap };
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * iBackBufferIndex;
	m_pGPUbridge->WaitForPreviousFrame(iBackBufferIndex);
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	pCL->SetDescriptorHeaps(1, ppDescriptorHeaps);
	pCL->SetComputeRootSignature(m_pComputeRS);
	pCL->SetPipelineState(m_pComputePSO);
	pCL->SetGraphicsRootSignature(m_pGraphicsRS);

	m_pMainCamera->BindCameraBuffer(0, pCL, iBackBufferIndex);
	pCL->SetComputeRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pSurfaceDescriptorHeap->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV));
	pCL->SetComputeRoot32BitConstant(2, iBackBufferIndex, 0);
	pCL->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pSurfaceDescriptorHeap->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV));
	
	//pCL->SetComputeRoot32BitConstant(2, D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), 1);

	pCL->Dispatch(1, 1, 1);

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
