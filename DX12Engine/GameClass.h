#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "TriangleObject.h"
#include "BoxObject.h"
#include "FrameBuffer.h"

class GameClass
{
public:
	GameClass();
	~GameClass();

	bool Initialize(FrameBuffer* pFrameBuffer);
	void Update();
	bool Render(FrameBuffer* pFrameBuffer);
	void CleanUp();


private:
	Camera* m_pMainCamera;
	PSOHandler* m_pPsoHandler;
	TriangleObject* tri;
	BoxObject* box;
	BoxObject* box1;

	
};

