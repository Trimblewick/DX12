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
	m_pPlaneObject = new Plane(m_pRenderer->temp_GetGraphicsCommandList_thisFrame());
	

	return true;
}
float gg = 3;

void GameClass::Update(Input* input, float dt)
{
	m_pMainCamera->Update(input, dt);

	m_pPlaneObject->Update(m_pMainCamera);

}


bool GameClass::Render()
{
	
	ID3D12GraphicsCommandList* pCL = m_pRenderer->temp_GetGraphicsCommandList_thisFrame();

	m_pRenderer->temp_TransitionCurrentBackBufferRTVIntoRenderState(pCL);
	
	//m_pPlaneObject->Draw(pCL, m_pMainCamera);
	
	m_pRenderer->temp_TransitionCurrentBackBufferRTVIntoPrecentState(pCL);
	

	DxAssert(pCL->Close(), S_OK);

	D3DClass::QueueGraphicsCommandList(pCL);
	D3DClass::ExecuteGraphicsCommandLists();
	
	DxAssert(D3DClass::GetSwapChain()->Present(0, 0), S_OK);
	D3DClass::WaitForPreviousFrame();
	SAFE_RELEASE(pCL);
	
	

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
	
}
