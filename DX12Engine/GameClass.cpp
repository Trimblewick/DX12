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




	tri = new TriangleObject(m_pPsoHandler);
}

void GameClass::Update()
{

}

bool GameClass::Render()
{
	D3DClass::WaitForPreviousFrame();

	D3DClass::GetCurrentCommandAllocator()->Reset();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(D3DClass::GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), D3DClass::GetFrameIndex(), D3DClass::GetRTVDescriptorSize());

	
	tri->Draw(&rtvHandle, m_pMainCamera);
	
	D3DClass::QueueGraphicsCommandList(tri->GetCommandList());

	D3DClass::ExecuteGraphicsCommandLists();
	
	if (!D3DClass::Render(m_pMainCamera, tri))
	{
		return false;
	}
	
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
	delete tri;

}
