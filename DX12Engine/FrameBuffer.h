#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include "Common.h"
#include "D3dClass.h"
#include "PSOHandler.h"
#include <memory>

class FrameBuffer
{
public:
	enum PIPELINES
	{
		STANDARD = 0
	};
private:
	const float m_fClearColor[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
public:
	FrameBuffer();
	~FrameBuffer();

	bool Initialize();
	void CleanUp();
	bool ResetList(PIPELINES i);// , __in_opt D3D12_CPU_DESCRIPTOR_HANDLE* pRTVHandle = NULL, __in_opt ID3D12PipelineState* pPSO);
	bool CloseList(PIPELINES i);


	void SetRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE* pRTVHandle);
	D3D12_CPU_DESCRIPTOR_HANDLE* GetRTVHandle();

	ID3D12GraphicsCommandList*								GetGraphicsCommandList(PIPELINES i);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE* _pRTVHandle;//pointer to allocated memory for the D3DClass::GetRTVDescriptorHande() function.

	std::vector<ID3D12GraphicsCommandList*>					m_pGraphicsCommandLists;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC>			m_pPSODescs;
	std::vector<ID3D12PipelineState*>						m_pPSOs;
	//PSOHandler											m_PSOHandler;

	std::vector<ID3D12Resource*>							m_pDepthStencilsBuffers;
	std::vector<ID3D12DescriptorHeap*>						m_pDepthStencilsDescriptorHeaps;


};

