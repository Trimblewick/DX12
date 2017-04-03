


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 


#include "SystemClass.h"

using namespace DirectX; // we will be using the directxmath library



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	MSG msg = { 0 };

	SystemClass::Initialize(hInstance, hPrevInstance, nShowCmd, 800, 600, L"WINDOW!");

	SystemClass::Run();

	SystemClass::CleanUp();


	_CrtDumpMemoryLeaks();
	return (int)msg.wParam;
}
