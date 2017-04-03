#include "SystemClass.h"


bool SystemClass::s_bRunning;
GameClass SystemClass::s_game;
bool SystemClass::s_initialized;

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
	if (s_initialized)
	{
		return false;
	}
	WindowClass::Initialize(hInstance, nCmdShow, windowWidth, windowHeight, title, false);
	s_game.Initialize(3);
	
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
		else 
		{
			// run game code
			//Update(); // update the game logic

			s_bRunning = s_game.Render();
			//D3DClass::Render(); // execute the command queue (rendering the scene is the result of the gpu executing the command lists)
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
	WindowClass::Destroy();
}
