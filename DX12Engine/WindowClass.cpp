#include "WindowClass.h"

LONG WindowClass::s_width;
LONG WindowClass::s_height;
float WindowClass::s_aspectRatio;
WNDCLASSEX WindowClass::s_windowClassInfo;
HWND WindowClass::s_windowHandler;
RECT WindowClass::s_windowRectangle;
LPWSTR WindowClass::s_title;
bool WindowClass::s_initialized;
bool WindowClass::s_visibleCursor;
WindowClass::Ratio WindowClass::s_ratio;
bool WindowClass::s_bFullscreen;


WindowClass::WindowClass()
{
}

WindowClass::~WindowClass()
{
}


LRESULT CALLBACK WindowClass::EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		return 0;
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		/*Input::ProcessKeyboard(message, wParam, lParam);
		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool WindowClass::UpdateWindow()
{
	return false;
}



bool WindowClass::Initialize(HINSTANCE hInstance, int nCmdShow, LONG width, LONG height, LPWSTR title, bool bFullscreen)
{
	if (s_initialized)
		return false;

	s_initialized = false;
	s_height = height;
	s_width = width;
	s_title = title;
	s_bFullscreen = bFullscreen;
	

	SetAspectRatio();

	s_windowClassInfo = { 0 };
	s_windowClassInfo.cbSize = sizeof(WNDCLASSEX);
	s_windowClassInfo.style = CS_HREDRAW | CS_VREDRAW;
	s_windowClassInfo.lpfnWndProc = EventHandler; //Callback for EVENTS
	s_windowClassInfo.hInstance = hInstance;
	s_windowClassInfo.lpszClassName = L"Window";
	s_windowClassInfo.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

	if (!RegisterClassEx(&s_windowClassInfo))
		return false;

	s_windowRectangle = { 0, 0, width, height };


	//Properties for window
	AdjustWindowRect(&s_windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	s_windowHandler = CreateWindow(
		L"Window",			// CLASS, if does not exists fails!
		title,		// Window name (title)
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		s_windowRectangle.right - s_windowRectangle.left,
		s_windowRectangle.bottom - s_windowRectangle.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);


	if (s_windowHandler)
	{
		s_initialized = true;
		ChangeWindowShowState(nCmdShow);
	}


	return s_initialized;
}

bool WindowClass::SetHeight(LONG height)
{
	if (height > 0 && height <= GetVerticalResolution())
	{
		s_height = height;
		return UpdateWindow();
	}

	return false;
}

bool WindowClass::SetWidth(LONG width)
{
	if (width > 0 && width <= GetHorizontalResolution())
	{
		s_width = width;
		return UpdateWindow();
	}

	return false;
}

bool WindowClass::SetSize(LONG height, LONG width)
{
	if (SetWidth(width) && SetHeight(height))
	{
		return true;
	}

	return false;
}

LONG WindowClass::GetHeight()
{
	return s_height;
}

LONG WindowClass::GetWidth()
{
	return s_width;
}

WindowClass::Ratio WindowClass::GetAspectRatio()
{
	return s_ratio;
}

HWND WindowClass::GetWindowHandler()
{
	return s_windowHandler;
}

LONG WindowClass::GetHorizontalResolution()
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return desktop.right;
}

LONG WindowClass::GetVerticalResolution()
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return desktop.bottom;
}

bool WindowClass::IsFullscreen()
{
	return s_bFullscreen;
}

bool WindowClass::SetAspectRatio()
{
	s_aspectRatio = float(s_width) / float(s_height);

	if (s_aspectRatio > 1.7f && s_aspectRatio < 1.8f)
	{
		s_ratio = Ratio::STANDARD_169;
	}
	else if (s_aspectRatio > 1.32f && s_aspectRatio < 1.34f)
	{
		s_ratio = Ratio::STANDARD_43;
	}
	else if (s_aspectRatio > 1.5f && s_aspectRatio < 1.7f)
	{
		s_ratio = Ratio::STANDARD_1610;
	}
	return true;
}

void WindowClass::ShowCursor()
{
	s_visibleCursor = true;
	UpdateWindow();
}

void WindowClass::HideCursor()
{
	s_visibleCursor = false;
	UpdateWindow();
}

bool WindowClass::Destroy()
{
	return DestroyWindow(s_windowHandler);
}

bool WindowClass::Initialized()
{
	return s_initialized;
}

bool WindowClass::ChangeWindowShowState(int nCmdShow)
{
	return ShowWindow(s_windowHandler, nCmdShow);
}

