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
	m_pPsoHandler = new PSOHandler();


	//tri = new TriangleObject(m_pPsoHandler);
	DirectX::XMFLOAT4 boxInitPos = DirectX::XMFLOAT4(2, 2, 4, 1);
	DirectX::XMFLOAT4 boxInitRot = DirectX::XMFLOAT4(0, 0, 0, 0);
	DirectX::XMFLOAT4 box1InitPos = DirectX::XMFLOAT4(-2, -2, 4, 1);
	box = new BoxObject(boxInitPos, boxInitRot, m_pPsoHandler, pFrameBuffer);
	box1 = new BoxObject(box1InitPos, boxInitRot, m_pPsoHandler, pFrameBuffer);
	
	
	
	return true;
}

void GameClass::Update()
{
	box->Update(m_pMainCamera);
	box1->Update(m_pMainCamera);
}

bool GameClass::Render(FrameBuffer* pFrameBuffer)
{
	
	box->Draw(pFrameBuffer, m_pMainCamera);
	box1->Draw(pFrameBuffer, m_pMainCamera);
	
	//D3DClass::QueueGraphicsCommandList(box->GetGraphicsCommandList());
	//D3DClass::QueueGraphicsCommandList(box1->GetGraphicsCommandList());


	
	return true;
}

void GameClass::CleanUp()
{
	if (m_pMainCamera)
	{
		delete m_pMainCamera;
		m_pMainCamera = nullptr;
	}
	if (m_pPsoHandler)
	{
		delete m_pPsoHandler;
		m_pPsoHandler = nullptr;
	}
	if (tri)
	{
		delete tri;
		tri = nullptr;
	}
	if (box)
	{
		delete box;
		box = nullptr;
	}
	if (box1)
	{
		delete box1;
	}
}
