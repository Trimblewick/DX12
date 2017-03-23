
#include "SystemClass.h"

using namespace DirectX; // we will be using the directxmath library



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	SystemClass::Initialize(hInstance, hPrevInstance, nShowCmd, 800, 600, L"WINDOW!");

	SystemClass::Run();

	SystemClass::CleanUp();

	return 0;
}
