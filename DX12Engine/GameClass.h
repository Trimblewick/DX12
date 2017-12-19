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


	D3D12_DESCRIPTOR_RANGE								m_pUAVDescriptorRanges[1];
	D3D12_ROOT_DESCRIPTOR_TABLE							m_UAVDescriptorTable;
	ID3D12Resource*										m_ppUAVTargets[3];

	ID3D12DescriptorHeap*								m_pSurfaceDescriptorHeap;
	ID3D12RootSignature*								m_pComputeRS;
	ID3D12RootSignature*								m_pGraphicsRS;
	ID3D12PipelineState*								m_pComputePSO;
	ID3D12PipelineState*								m_pGraphicsPSO;


	GPUbridge*											m_pGPUbridge;
	ResourceLoader*										m_pResourceLoader;
};
 
