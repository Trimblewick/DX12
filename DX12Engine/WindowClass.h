#pragma once

#include <Windows.h>
#include <d3d12.h>


class WindowClass
{
public:
	enum class Ratio
	{
		STANDARD_169 = 0,
		STANDARD_1610 = 1,
		STANDARD_43 = 2,
	};

private:
	static bool UpdateWindow();
public:
	WindowClass();
	~WindowClass();

	static bool Initialize(HINSTANCE hInstance, int nCmdShow, LONG width, LONG height, LPWSTR title, bool bFullscreen, WNDPROC EventHandler);

	static bool SetHeight(LONG height);
	static bool SetWidth(LONG width);
	static bool SetAspectRatio();
	static bool SetSize(LONG height, LONG width);

	static LONG GetHeight();
	static LONG GetWidth();
	static Ratio GetAspectRatio();
	static HWND GetWindowHandler();
	static HINSTANCE GetHinstance();
	static LONG GetHorizontalResolution();
	static LONG GetVerticalResolution();
	static bool IsFullscreen();

	static void ShowCursor();
	static void HideCursor();
	static bool Destroy();
	static bool Initialized();

	static bool ChangeWindowShowState(int nCmdShow);

private:
	static LONG s_width;
	static LONG s_height;
	static float s_aspectRatio;
	static WNDCLASSEX s_windowClassInfo;
	static HWND s_windowHandler;
	static HINSTANCE s_hinstance;
	static RECT s_windowRectangle;
	static LPWSTR s_title;
	static bool s_initialized;
	static bool s_visibleCursor;
	static Ratio s_ratio;
	static bool s_bFullscreen;

};

