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
	bool Frame();

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE hInstance;
	LPWSTR lpCmdLine;
	int nCmdShow;
	HWND hWnd;
	MSG msg;

	struct screen_dimensionsXY
	{
		float screenWidth;//X
		float screenHeight;//Y
	} *screendimensions;

	Input* input;
	GraphicsClass* graphics;
};


#endif // !SYSTEMCLASS_H

