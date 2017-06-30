#pragma once
#include "Common.h"
#include "D3dClass.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "Texture.h"
#include <random>
#include <ctime>
#include "FrustumCulling.h"

class GrassBlades
{
private:
	struct StructGrass
	{
		DirectX::XMFLOAT4 position[4];
		DirectX::XMFLOAT4 binormal;
		DirectX::XMFLOAT4 seed;
	};

	struct WVPMatrixBufferStruct
	{
		DirectX::XMFLOAT4X4 wvpMat;
	};
public:
	GrassBlades();
	~GrassBlades();

	void Update(Camera* camera);
	void Draw(FrameBuffer* pFrameBuffer, Camera* camera , FrustumCulling* pFrustumCuller);

private:
	unsigned int						m_uiGrassInstances; //nr of straws
	int									m_iGridDim;//width of hightmap basically
	int**								m_ppTiles;

	ID3D12CommandAllocator*				m_pCA;
	ID3D12GraphicsCommandList*			m_pCL;

	ID3D12PipelineState*				m_pPSO;

	ID3D12RootSignature*				m_pRootSignature;
	
	ID3D12Resource*						m_pTextureHeightMap;
	ID3D12DescriptorHeap*				m_pDHHeightMap;
	int									m_iDHsize;

	ID3D12Resource*						m_pBufferGrassBladesList;
	D3D12_UNORDERED_ACCESS_VIEW_DESC	m_UAVdescGrassBlades;


	WVPMatrixBufferStruct				m_wvpMat;
	ID3D12Resource*						m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*								m_pWVPGPUAdress[g_cFrameBufferCount];

	Texture*							m_pHeightMap;

};
