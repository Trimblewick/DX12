#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	
}

GraphicsClass::~GraphicsClass()
{
	// Release the Direct3D object.
	if (m_pDirect3D)
	{
		m_pDirect3D->Shutdown();
		delete m_pDirect3D;
		m_pDirect3D = 0;
	}
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_pDirect3D = new D3dClass();
	if (!m_pDirect3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_pDirect3D->Initialize(screenWidth, screenHeight, hwnd, true, false);
	assert(result);

	return true;
}

bool GraphicsClass::Render()
{
	bool result;


	// Use the Direct3D object to render the scene.
	result = m_pDirect3D->Render();
	assert(result);

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