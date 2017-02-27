#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	
}

GraphicsClass::~GraphicsClass()
{
	// Release the Direct3D object.
	if (this->direct3D)
	{
		this->direct3D->Shutdown();
		delete this->direct3D;
		this->direct3D = 0;
	}
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	this->direct3D = new D3dClass;
	if (!this->direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = this->direct3D->Initialize(screenWidth, screenHeight, hwnd, true, true);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	bool result;


	// Use the Direct3D object to render the scene.
	result = direct3D->Render();
	if (!result)
	{
		return false;
	}



	return true;
}

bool GraphicsClass::Frame()
{
	bool result;


	// Render the graphics scene.
	result = this->Render();
	if (!result)
	{
		return false;
	}

	return true;
}