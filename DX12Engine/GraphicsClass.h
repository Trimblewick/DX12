#ifndef GRAPHICSCLASS_H
#define GRAPHICSCLASS_H

#include "Common.h"
#include "D3dClass.h"


class GraphicsClass
{
public:
	GraphicsClass();
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	
	bool Frame();
private:
	bool Render();
	D3dClass* m_pDirect3D;
};



#endif // !GraphicsClass.h
