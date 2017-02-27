#include "SystemClass.h"

static SystemClass* applicationhandle;

SystemClass::SystemClass()
{
	//init here?
	this->input = new Input();
	this->graphics = new GraphicsClass();
	
}

SystemClass::~SystemClass()
{
	delete this->screendimensions;
	DestroyWindow(this->hWnd);
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		this->input->KeyDown((unsigned int)wParam);
		return 0;
	case WM_KEYUP:
		this->input->KeyUp((unsigned int)wParam);
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
	//initialize window
	this->hInstance = GetModuleHandle(NULL);

	applicationhandle = this;

	WNDCLASSEX wcx = { 0 };
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WindowProcedure;
	wcx.hInstance = hInstance;
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

	this->screendimensions = new screen_dimensionsXY;

	this->screendimensions->screenWidth = 1280.0f;
	this->screendimensions->screenHeight = 720.0f;
	
	this->hWnd = CreateWindow(wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, 100, 100, 
		this->screendimensions->screenWidth, this->screendimensions->screenHeight, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(this->hWnd, SW_SHOW);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);

	this->graphics->Initialize(screendimensions->screenWidth, screendimensions->screenHeight, hWnd);

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
	this->msg = { 0 };
	

	while (WM_QUIT != msg.message)
	{


		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			bool result = this->Frame();
			if (!result)
			{
				this->msg.message = WM_QUIT;
			}
		}
	}

	return;
}



bool SystemClass::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if (!input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = this->graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}