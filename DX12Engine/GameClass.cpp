#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

bool GameClass::Initialize()
{
	m_pMainCamera = new Camera(DirectX::XMFLOAT3(-20.0f, 34.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pRenderer = new DeferredRenderer();
	m_pGPUbridge = new GPUbridge();

	m_pRenderer->Initialize(m_pGPUbridge->GetCQ());

	m_pPlaneObject = new Plane(m_pGPUbridge, m_pRenderer->GetSwapChain());

	m_pResourceLoader = new ResourceLoader();
	Mesh* pMesh = m_pResourceLoader->LoadMeshFromFile("../Resources/Teapot/teapot_n_glass.obj", Mesh::MeshLayout::VERTEXNORMAL, m_pGPUbridge);

	return true;
}

void GameClass::Update(Input* input, float dt)
{
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	m_pMainCamera->Update(input, dt);

	m_pPlaneObject->Update(m_pMainCamera, iBackBufferIndex);
}

bool GameClass::Render()
{
	m_pGPUbridge->WaitForPreviousFrame(m_pRenderer->GetBackBufferIndex());
	
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	
	m_pRenderer->RenderLightPass(pCL);

	m_pPlaneObject->Draw(pCL, m_pMainCamera, m_pRenderer->GetBackBufferIndex());

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
}
