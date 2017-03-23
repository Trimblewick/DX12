#pragma once

#include "D3dClass.h"
#include "Camera.h"

class GameClass
{
public:
	GameClass();
	~GameClass();

	void Initialize(int cFrameBufferCount);
	void Update();
	bool Render();
	void CleanUp();
private:
	Camera* m_pMainCamera;
};

