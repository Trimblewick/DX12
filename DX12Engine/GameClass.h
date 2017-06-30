#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "FrameBuffer.h"

#include "Input.h"
#include "BoxObject.h"
#include "Plane.h"
#include "GrassBlades.h"
#include "FrustumCulling.h"

class GameClass
{
public:
	GameClass();
	~GameClass();

	bool Initialize(FrameBuffer* pFrameBuffer);
	void Update(Input* input);
	bool Render(FrameBuffer* pFrameBuffer);
	void CleanUp();


private:
	Camera*					m_pMainCamera;
	FrustumCulling*			m_pFrustumCuller;

	Plane*					m_pPlaneObject;
	GrassBlades*			m_pGrassBlades;
};

