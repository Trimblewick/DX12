#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include "Common.h"
#include "D3dClass.h"
#include "PSOHandler.h"


class FrameBuffer
{
public:
	enum PIPELINES
	{
		STANDARD = 0
	};

public:
	FrameBuffer();
	~FrameBuffer();

	void Initialize();

	ID3D12GraphicsCommandList*								m_GetGraphicsCommandList(PIPELINES i);

private:
	std::vector<ID3D12GraphicsCommandList*>					m_pGraphicsCommandLists;

	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC>			m_pPSODescs;
	std::vector<ID3D12PipelineState*>						m_pPSOs;

	std::vector<ID3D12Resource*>							m_pDepthStencilsBuffers;
	std::vector<ID3D12DescriptorHeap*>						m_pDepthStencilsDescriptorHeaps;


};

