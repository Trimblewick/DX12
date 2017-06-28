#pragma once
#include "Common.h"
#include "D3dClass.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include <random>
#include <ctime>

class GrassBlades
{
private:
	struct StructGrass
	{
		/*Vertex() : position(0, 0, 0) {}
		Vertex(float x, float y, float z) : position(x, y, z) {}*/
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
	void Draw(FrameBuffer* pFrameBuffer, Camera* camera);

private:
	unsigned int						m_uiGrassInstances; //nr of straws * nr of section in straws --> for the drawcall

	ID3D12CommandAllocator*				m_pCA;
	ID3D12GraphicsCommandList*			m_pCL;

	ID3D12PipelineState*				m_pPSO;

	ID3D12RootSignature*				m_pRootSignature;
	

	ID3D12Resource*						m_pBufferGrassBladesList;
	
	D3D12_UNORDERED_ACCESS_VIEW_DESC	m_UAVdescGrassBlades;
	ID3D12DescriptorHeap*				m_pDHGrassBlade;
	UINT								m_uiDHsize;
	CD3DX12_ROOT_PARAMETER*				m_pRootParameters[1];
	CD3DX12_DESCRIPTOR_RANGE*			m_pDHrange[1];


	WVPMatrixBufferStruct					m_wvpMat;
	ID3D12Resource*							m_pWVPMatUpploadHeaps[g_cFrameBufferCount];
	UINT8*									m_pWVPGPUAdress[g_cFrameBufferCount];

};
