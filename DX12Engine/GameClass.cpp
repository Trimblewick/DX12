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

	//CREATE SHADER
	Shader* pComputeShader = m_pResourceLoader->CreateComputeShader(L"RayTracerCS.hlsl");

	
	
	D3D12_ROOT_PARAMETER cameraRootParameter;
	cameraRootParameter.Descriptor = CD3DX12_ROOT_DESCRIPTOR(0, 0);
	cameraRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	cameraRootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_PARAMETER rtvRP;
	rtvRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rtvRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rtvRP.DescriptorTable = m_pRenderer->GetRTVDescriptorTable();

	D3D12_ROOT_PARAMETER indexRP;
	indexRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	indexRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	indexRP.Constants = CD3DX12_ROOT_CONSTANTS(2, 1);

	pComputeShader->AddRootParameter(cameraRootParameter);
	pComputeShader->AddRootParameter(rtvRP);
	pComputeShader->AddRootParameter(indexRP);
	
	m_pRS = D3DClass::CreateRS(pComputeShader);
	m_pPSO = D3DClass::CreateComputePSO(pComputeShader, m_pRS);

	delete pComputeShader;

	return true;
}

void GameClass::Update(Input* input, float dt)
{
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	m_pMainCamera->Update(input, dt, iBackBufferIndex);

}

bool GameClass::Render()
{
	ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pRenderer->GetDH() };
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	m_pGPUbridge->WaitForPreviousFrame(iBackBufferIndex);
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	pCL->SetDescriptorHeaps(1, ppDescriptorHeaps);
	pCL->SetComputeRootSignature(m_pRS);
	pCL->SetPipelineState(m_pPSO);
	
	m_pMainCamera->BindCameraBuffer(0, pCL, iBackBufferIndex);
	pCL->SetComputeRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pRenderer->GetRTVDHhandle()));
	pCL->SetComputeRoot32BitConstant(2, iBackBufferIndex, 0);
	pCL->SetComputeRoot32BitConstant(2, D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), 1);

	pCL->Dispatch(1, 1, 1);

	pCL->Close();
	
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
