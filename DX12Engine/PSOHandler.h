#pragma once

#include <d3d12.h>
#include <vector>
#include "Common.h"


class PSOHandler
{
private:
	friend bool operator== (const D3D12_GRAPHICS_PIPELINE_STATE_DESC lhs, const D3D12_GRAPHICS_PIPELINE_STATE_DESC rhs);
	
public:
	PSOHandler();
	~PSOHandler();

	ID3D12PipelineState* CreatePipelineStateObject(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pPSODesc, ID3D12Device* pDevice);

private:
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC> m_vPSODescs;

	std::vector<ID3D12PipelineState*> m_vPSOs;

	
};

