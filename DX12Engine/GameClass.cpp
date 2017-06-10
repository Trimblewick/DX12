#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

bool GameClass::Initialize(FrameBuffer* pFrameBuffer)
{
	m_pMainCamera = new Camera();

	planeObject = new Plane(pFrameBuffer);

	
	return true;
}

void GameClass::Update()
{
	planeObject->Update(m_pMainCamera);
}

bool GameClass::Render(FrameBuffer* pFrameBuffer)
{
	planeObject->Draw(pFrameBuffer, m_pMainCamera);
	
	return true;
}

void GameClass::CleanUp()
{
	if (m_pMainCamera)
	{
		delete m_pMainCamera;
		m_pMainCamera = nullptr;
	}
	if (planeObject)
	{
		delete planeObject;
		planeObject = nullptr;
	}
	
}
