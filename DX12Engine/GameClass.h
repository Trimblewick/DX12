#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "FrameBuffer.h"


#include "BoxObject.h"
#include "Plane.h"

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

	BoxObject* box;
	BoxObject* box1;

	Plane* planeObject;
};

