#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

bool GameClass::Initialize(FrameBuffer* pFrameBuffer)
{
	m_pMainCamera = new Camera(DirectX::XMFLOAT3(3.0f, 10.0f, -10.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	m_pPlaneObject = new Plane(pFrameBuffer);
	m_pGrassBlades = new GrassBlades();
	
	return true;
}

void GameClass::Update(Input* input)
{
	m_pMainCamera->Update(input);

	m_pPlaneObject->Update(m_pMainCamera);
	m_pGrassBlades->Update(m_pMainCamera);
}

bool GameClass::Render(FrameBuffer* pFrameBuffer)
{
	m_pPlaneObject->Draw(pFrameBuffer, m_pMainCamera);
	m_pGrassBlades->Draw(pFrameBuffer, m_pMainCamera);
	
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
	if (m_pGrassBlades)
	{
		delete m_pGrassBlades;
		m_pGrassBlades = nullptr;
	}
}
