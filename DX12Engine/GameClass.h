#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "TriangleObject.h"

class GameClass
{
public:
	GameClass();
	~GameClass();

	void Initialize(int cFrameBufferCount);
	void Update();
	bool Render();
	void CleanUp();

	PSOHandler* GetPSOHander();

private:
	Camera* m_pMainCamera;
	PSOHandler* m_pPsoHandler;
	TriangleObject* tri;
};

