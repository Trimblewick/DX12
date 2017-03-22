#include "SystemClass.h"


bool SystemClass::s_bRunning;

SystemClass::SystemClass()
{
	s_bRunning = false;
}

SystemClass::~SystemClass()
{
}

void SystemClass::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, LONG windowWidth, LONG windowHeight, LPWSTR title)
{
	WindowClass::Initialize(hInstance, nCmdShow, windowWidth, windowHeight, title, false);
	D3DClass::Initialize(3);
	s_bRunning = true;
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
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// run game code
			//Update(); // update the game logic
			D3DClass::Render(); // execute the command queue (rendering the scene is the result of the gpu executing the command lists)
		}
	}
}

void SystemClass::SystemPause()
{
}

void SystemClass::SystemStop()
{
}
