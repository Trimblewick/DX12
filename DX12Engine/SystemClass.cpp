#include "SystemClass.h"

static SystemClass* applicationhandle;

SystemClass::SystemClass()
{
	
}

SystemClass::~SystemClass()
{
	DestroyWindow(m_hWnd);
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
	wcx.lpszClassName = L"Våldeboll";
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

	//////////////////////////////////////////////////created window, make private function

/*
	if (!hWnd)// creation of window failed, throw some exception
	{
		throw ExceptionHandler::initializationFailiure;
		return;
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill the swap chain description struct
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = this->hWnd;                          // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode

		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, 
			&scd, &this->swapChain, &this->device, NULL, &this->context);



	}*/

	
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