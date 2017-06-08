#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers.
#endif

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>
#include "Common.h"

#include "WindowClass.h"
#include "D3dClass.h"
#include "GameClass.h"
#include "Camera.h"

class SystemClass
{
public:
	SystemClass();
	~SystemClass();

	static bool Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, LONG windowWidth, LONG windowHeight, LPWSTR title);
	static void Run();
	static void Pause();
	static void Stop();
	static void CleanUp();


private:
	static FrameBuffer s_frameBuffer;
	static GameClass s_game;
	static bool s_bRunning;
	static bool s_initialized;
};
