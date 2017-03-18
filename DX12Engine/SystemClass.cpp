#include "SystemClass.h"

static SystemClass* applicationhandle;

SystemClass::SystemClass()
{
	
}

SystemClass::~SystemClass()
{
	
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		m_pInput->KeyDown((unsigned int)wParam);
		return 0;
	case WM_KEYUP:
		m_pInput->KeyUp((unsigned int)wParam);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return applicationhandle->MessageHandler(hWnd, message, wParam, lParam);
	}
}

bool SystemClass::Initialize()
{
	m_pInput = new Input();
	m_pGraphics = new GraphicsClass();
	//initialize window
	m_hInstance = GetModuleHandle(NULL);

	applicationhandle = this;

	WNDCLASSEX wcx = { 0 };
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WindowProcedure;
	wcx.hInstance = this->m_hInstance;
	wcx.lpszClassName = L"DX12";
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wcx.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(0, IDC_ARROW);
	wcx.lpszMenuName = NULL;

	if (!RegisterClassEx(&wcx))
	{
		return false;
	}


	m_fScreenWidth = 1280.0f;
	m_fScreenHeight = 720.0f;
	
	m_hWnd = CreateWindow(wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, 100, 100, 
		m_fScreenWidth, m_fScreenHeight, nullptr, nullptr, m_hInstance, nullptr);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	this->m_pGraphics->Initialize(m_fScreenWidth, m_fScreenHeight, m_hWnd);

	return true;
}

void SystemClass::Run() throw()
{
	bool result;
	m_msg = { 0 };
	

	while (WM_QUIT != m_msg.message)
	{


		if (PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
		else
		{
			bool result = Frame();
			if (!result)
			{
				m_msg.message = WM_QUIT;
			}
		}
	}

	return;
}

void SystemClass::Shutdown()
{
	DestroyWindow(m_hWnd);
	
}



bool SystemClass::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if (!m_pInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_pGraphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}