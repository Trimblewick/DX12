#pragma once

#include <Windows.h>

class Window
{
public:
	Window();
	~Window();

	bool InitWindow(HINSTANCE hInstance, int ShowWnd, bool fullscreen);

private:
	HWND m_hwnd;
	LPCTSTR m_windowName;
	LPCTSTR m_windowTitle;

	unsigned int m_width;
	unsigned int m_height;



};
/*
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				//Running = false;
				DestroyWindow(hwnd);
			}
		}
		return 0;

	case WM_DESTROY: // x button on top right corner of window was pressed
		//Running = false;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}*/
