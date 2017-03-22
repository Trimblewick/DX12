#include "SystemClass.h"

SystemClass::SystemClass()
{
}

SystemClass::~SystemClass()
{
}

void SystemClass::SystemInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, LONG windowWidth, LONG windowHeight, LPWSTR title)
{
	WindowClass::Initialize(hInstance, nCmdShow, windowWidth, windowHeight, title, false);
	//s_d3d.Initialize(s_mainWindow, 3);

}

void SystemClass::SystemRun()
{
}

void SystemClass::SystemPause()
{
}

void SystemClass::SystemStop()
{
}
