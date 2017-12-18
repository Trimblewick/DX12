#pragma once

#include "D3dClass.h"
#include "Camera.h"
#include "DeferredRenderer.h"
#include "Input.h"
#include "GPUbridge.h"
#include "ResourceLoader.h"

#include "Object.h"
#include "Pipeline.h"

//#include "Plane.h"
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

	ID3D12RootSignature*								m_pRS;
	ID3D12PipelineState*								m_pPSO;


	GPUbridge*											m_pGPUbridge;
	ResourceLoader*										m_pResourceLoader;
};
 
