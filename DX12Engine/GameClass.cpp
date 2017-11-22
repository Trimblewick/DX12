#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

bool GameClass::Initialize()
{
	m_pMainCamera = new Camera(DirectX::XMFLOAT3(-5.0f, 5.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pRenderer = new DeferredRenderer();
	m_pGPUbridge = new GPUbridge();
	m_pResourceLoader = new ResourceLoader();

	m_pRenderer->Initialize(m_pGPUbridge->GetCQ());
	
	
	m_pPlaneObject = new Plane(m_pGPUbridge, m_pRenderer->GetSwapChain());

	//LOAD MESH
	m_pObject = new Object();
	m_pObject->SetMesh(m_pResourceLoader->LoadMeshFromFile("../Resources/Teapot/teapot_n_glass.obj", Mesh::MeshLayout::VERTEXNORMAL, m_pGPUbridge));

	//CREATE SHADER

	Shader* pShader = m_pResourceLoader->CreateShader(L"VertexShader.hlsl", L"PixelShader.hlsl");

	D3D12_INPUT_ELEMENT_DESC inputLayoutElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};


	//fill input layout desc
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayoutElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayoutElementDesc;
	
	pShader->SetInputLayout(&inputLayoutDesc);
	
	
	D3D12_ROOT_PARAMETER cameraRootParameter;
	cameraRootParameter.Descriptor = CD3DX12_ROOT_DESCRIPTOR(0, 0);
	cameraRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	cameraRootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pShader->AddRootParameter(cameraRootParameter);

	//SUPPLY PIPELINE

	Pipeline* pP = new Pipeline();
	
	ID3D12RootSignature* pRS = D3DClass::CreateRS(pShader);

	pP->SetRS(pRS);
	pP->SetPSO(D3DClass::CreateGraphicsPSO(pShader, pRS));
	
	pP->AddObject(m_pObject);

	m_vPipelines.push_back(pP);

	return true;
}

void GameClass::Update(Input* input, float dt)
{
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	//m_pMainCamera->Update(input, dt, iBackBufferIndex);

//	m_pPlaneObject->Update(m_pMainCamera, iBackBufferIndex);
}

bool GameClass::Render()
{
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	m_pGPUbridge->WaitForPreviousFrame(iBackBufferIndex);
	
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	
	m_pRenderer->RenderLightPass(pCL);

	//m_pPlaneObject->Draw(pCL, m_pMainCamera, m_pRenderer->GetBackBufferIndex());
	//m_pMainCamera->BindCameraBuffer(0, pCL, iBackBufferIndex);
	
	for (Pipeline* pPipeline : m_vPipelines)
	{
		pPipeline->DrawObjects(pCL, m_pMainCamera, iBackBufferIndex);
	}
	
	m_pRenderer->temp_closelistNqueue(pCL);

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
	if (m_pPlaneObject)
	{
		delete m_pPlaneObject;
		m_pPlaneObject = nullptr;
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
	if (m_pObject)
	{
		delete m_pObject;
		m_pObject = nullptr;
	}
}
