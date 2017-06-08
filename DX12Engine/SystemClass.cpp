#include "SystemClass.h"


bool SystemClass::s_bRunning;
GameClass SystemClass::s_game;
bool SystemClass::s_initialized;
FrameBuffer SystemClass::s_frameBuffer;

SystemClass::SystemClass()
{
	s_bRunning = false;
	s_initialized = false;
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, LONG windowWidth, LONG windowHeight, LPWSTR title)
{
	if (s_initialized)//already initialized
	{
		return false;
	}
	if (!WindowClass::Initialize(hInstance, nCmdShow, windowWidth, windowHeight, title, false))
	{
		return false;
	}
	if (!D3DClass::Initialize())
	{
		return false;
	}
	if (!s_frameBuffer.Initialize())
	{
		return false;
	}
	if (!s_game.Initialize(&s_frameBuffer))
	{
		return false;
	}
	
	s_bRunning = true;
	s_initialized = true;
	return true;
}

void SystemClass::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (s_bRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				SystemClass::Stop();
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else //loop logics
		{
			D3DClass::WaitForPreviousFrame();
			DxAssert(D3DClass::GetCurrentCommandAllocator()->Reset(), S_OK);
			


			s_game.Update();



			s_frameBuffer.ResetList(FrameBuffer::PIPELINES::STANDARD);
			
			s_bRunning = s_game.Render(&s_frameBuffer);

			s_frameBuffer.CloseList(FrameBuffer::PIPELINES::STANDARD);

			D3DClass::ExecuteGraphicsCommandLists();
			DxAssert(D3DClass::GetSwapChain()->Present(0, 0), S_OK);
		}
	}
	
	
}

void SystemClass::Pause()
{
}

void SystemClass::Stop()
{
	s_bRunning = false;
}

void SystemClass::CleanUp()
{
	s_game.CleanUp();
	s_frameBuffer.CleanUp();
	WindowClass::Destroy();
	D3DClass::Cleanup();
}
