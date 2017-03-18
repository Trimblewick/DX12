#ifndef SYSTEMCLASS_H
#define SYSTEMCLASS_H
#define WIN32_LEAN_AND_MEAN


#include <Windows.h>
#include "Input.h"
#include "GraphicsClass.h"



class SystemClass 
{
public:
	SystemClass();
	~SystemClass();

	bool Initialize();
	void Run();
	void Shutdown();
	bool Frame();

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE m_hInstance;
	LPWSTR m_lpCmdLine;
	int m_nCmdShow;
	HWND m_hWnd;
	MSG m_msg;

	
	float m_fScreenWidth;
	float m_fScreenHeight;
	

	Input* m_pInput;
	GraphicsClass* m_pGraphics;
};


#endif // !SYSTEMCLASS_H

