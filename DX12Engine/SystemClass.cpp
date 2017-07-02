#include "SystemClass.h"


bool SystemClass::s_bRunning;
GameClass SystemClass::s_game;
bool SystemClass::s_initialized;
FrameBuffer SystemClass::s_frameBuffer;
Input SystemClass::s_input;
float SystemClass::s_fDeltaTime;

SystemClass::SystemClass()
{
	s_bRunning = false;
	s_initialized = false;
	s_fDeltaTime = 0.0f;
}

SystemClass::~SystemClass()
{
}


LRESULT CALLBACK SystemClass::EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//If one case is hit the code will execute everything down until a break;
	switch (message)
	{

	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		//Input::ProcessGamePad(message, wParam, lParam);
		//break;
	case WM_ACTIVATEAPP:
		/*Input::ProcessKeyboard(message, wParam, lParam);
		Input::ProcessMouse(message, wParam, lParam);
		break;*/
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		/*Input::ProcessMouse(message, wParam, lParam);
		break;*/
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
			}
		}
		else
		{
			s_input.KeyDown((unsigned int)wParam);
		}
		return 0;
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
		s_input.KeyUp((unsigned int)wParam);
		return 0;
	case WM_SYSKEYUP:
		/*Input::ProcessKeyboard(message, wParam, lParam);
		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SystemClass::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, LONG windowWidth, LONG windowHeight, LPWSTR title)
{
	if (s_initialized)//already initialized
	{
		return false;
	}
	if (!WindowClass::Initialize(hInstance, nCmdShow, windowWidth, windowHeight, title, false, EventHandler))
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
	if (!s_input.Initialize())
	{
		return false;
	}
	
	s_bRunning = true;
	s_initialized = true;
	return true;
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void SystemClass::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	auto prevTime = std::chrono::steady_clock::now();
	auto currentTime = std::chrono::steady_clock::now();
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
			auto currentTime = std::chrono::steady_clock::now();
			s_fDeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - prevTime).count() / 1000000.0f;
			prevTime = currentTime;

			float fps = 1.0f / s_fDeltaTime;
			
			std::string stFPS = "FPS: " + std::to_string(fps);
			std::wstring stemp = s2ws(stFPS);
			LPCWSTR st = stemp.c_str();
			WindowClass::SetWindowTitle(st);

			DxAssert(D3DClass::GetCurrentCommandAllocator()->Reset(), S_OK);
			
			

			s_game.Update(&s_input, s_fDeltaTime);



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
