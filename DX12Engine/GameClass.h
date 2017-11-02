#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "DeferredRenderer.h"
#include "Input.h"


#include "GPUbridge.h"
//#include "BoxObject.h"

#include "ResourceLoader.h"
#include "Plane.h"


//#include "GrassBlades.h"
//#include "FrustumCulling.h"
//#include "SkyBox.h"

class GameClass
{
public:
	GameClass();
	~GameClass();

	bool Initialize();
	void Update(Input* input, float dt);
	bool Render();
	void CleanUp();
	

private:
	Camera*												m_pMainCamera;
	DeferredRenderer*									m_pRenderer;
	Plane*												m_pPlaneObject;

	GPUbridge*											m_pGPUbridge;

	ResourceLoader*	m_pResourceLoader;
};
 
