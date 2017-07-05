#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "FrameBuffer.h"

#include "Input.h"
#include "BoxObject.h"
#include "Plane.h"
#include "GrassBlades.h"
#include "FrustumCulling.h"
#include "SkyBox.h"

class GameClass
{
public:
	GameClass();
	~GameClass();

	bool Initialize(FrameBuffer* pFrameBuffer);
	void Update(Input* input, float dt);
	bool Render(FrameBuffer* pFrameBuffer);
	void CleanUp();


private:
	Camera*					m_pMainCamera;
	SkyBox*					m_pSkyBox;
	FrustumCulling*			m_pFrustumCuller;

	Plane*					m_pPlaneObject;
	GrassBlades*			m_pGrassBlades;
};

