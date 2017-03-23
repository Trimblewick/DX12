#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

void GameClass::Initialize(int cFrameBufferCount)
{
	m_pMainCamera = new Camera();
	m_pPsoHandler = new PSOHandler();
	D3DClass::Initialize(3, m_pPsoHandler);

}

void GameClass::Update()
{

}

bool GameClass::Render()
{
	
	if (!D3DClass::Render(m_pMainCamera))
	{
		return false;
	}
	return true;
}

void GameClass::CleanUp()
{
	delete m_pMainCamera;
	delete m_pPsoHandler;
	m_pMainCamera = nullptr;
}

PSOHandler * GameClass::GetPSOHander()
{
	return m_pPsoHandler;
}

