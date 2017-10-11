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
	m_pPlaneObject = new Plane();
	

	for (int i = 0; i < 3; ++i)
	{
		temp_ca[i] = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	
	return true;
}

void GameClass::Update(Input* input, float dt)
{
	m_pMainCamera->Update(input, dt);

	m_pPlaneObject->Update(m_pMainCamera);

}

bool GameClass::Render()
{
	m_pGPUbridge->WaitForPreviousFrame();
	//ID3D12GraphicsCommandList* pCL = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, temp_ca[D3DClass::GetFrameIndex()]);//m_pRenderer->temp_GetGraphicsCommandList_thisFrame();
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	m_pRenderer->RenderLightPass(pCL);

	//m_pRenderer->temp_setRendertarget(pCL);
	m_pPlaneObject->Draw(pCL, m_pMainCamera);

	m_pRenderer->temp_closelistNqueue(pCL);

	m_pGPUbridge->QueueGraphicsCL(pCL);
	m_pGPUbridge->ExecuteGrapichsCLs();

	m_pRenderer->temp_swap();
//	DxAssert(D3DClass::GetSwapChain()->Present(0, 0), S_OK);
	

	return true;
}

void GameClass::CleanUp()
{
	if (m_pMainCamera)
	{
		delete m_pMainCamera;
		m_pMainCamera = nullptr;
	}
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
	if (m_pPlaneObject)
	{
		delete m_pPlaneObject;
		m_pPlaneObject = nullptr;
	}
	
}
