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
	D3DClass::Initialize(3);

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
}

