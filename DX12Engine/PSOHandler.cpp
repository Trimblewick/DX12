#include "PSOHandler.h"


PSOHandler::PSOHandler()
{
	
}

PSOHandler::~PSOHandler()
{
	for (ID3D12PipelineState* pso : m_vPSOs)
	{
		pso->Release();
	}
	m_vPSOs.clear();
	m_vPSODescs.clear();
}

bool operator==(const D3D12_GRAPHICS_PIPELINE_STATE_DESC lhs, const D3D12_GRAPHICS_PIPELINE_STATE_DESC rhs)
{
	if (lhs.pRootSignature != rhs.pRootSignature) return false;
	if (lhs.VS.BytecodeLength == rhs.VS.BytecodeLength) return false;
	if (lhs.VS.pShaderBytecode != rhs.VS.pShaderBytecode) return false;
	if (lhs.PrimitiveTopologyType != rhs.PrimitiveTopologyType) return false;
	if (lhs.NumRenderTargets != rhs.NumRenderTargets) return false;
	if (lhs.RTVFormats != rhs.RTVFormats) return false;
	if (lhs.SampleDesc.Count != rhs.SampleDesc.Count) return false;
	if (lhs.SampleDesc.Quality != rhs.SampleDesc.Quality) return false;
	if (lhs.BlendState.AlphaToCoverageEnable != rhs.BlendState.AlphaToCoverageEnable) return false;
	if (lhs.BlendState.IndependentBlendEnable != rhs.BlendState.IndependentBlendEnable) return false;
	if (lhs.BlendState.RenderTarget != rhs.BlendState.RenderTarget) return false;
	if (lhs.SampleMask != rhs.SampleMask) return false;
	if (lhs.RasterizerState.AntialiasedLineEnable != rhs.RasterizerState.AntialiasedLineEnable) return false;
	if (lhs.RasterizerState.ConservativeRaster != lhs.RasterizerState.ConservativeRaster) return false;
	if (lhs.RasterizerState.CullMode != rhs.RasterizerState.CullMode) return false;
	if (lhs.RasterizerState.DepthBias != rhs.RasterizerState.DepthBias) return false;
	if (lhs.RasterizerState.DepthBiasClamp != rhs.RasterizerState.DepthBiasClamp) return false;
	if (lhs.RasterizerState.DepthClipEnable != rhs.RasterizerState.DepthClipEnable) return false;
	if (lhs.RasterizerState.FillMode != rhs.RasterizerState.FillMode) return false;
	if (lhs.RasterizerState.ForcedSampleCount != rhs.RasterizerState.ForcedSampleCount) return false;
	if (lhs.RasterizerState.FrontCounterClockwise != rhs.RasterizerState.FrontCounterClockwise) return false;
	if (lhs.RasterizerState.MultisampleEnable != rhs.RasterizerState.MultisampleEnable) return false;
	if (lhs.RasterizerState.SlopeScaledDepthBias != rhs.RasterizerState.SlopeScaledDepthBias) return false;

	return true;
}


ID3D12PipelineState* PSOHandler::CreatePipelineStateObject(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pPSODesc, ID3D12Device* pDevice)
{

	for (unsigned int i = 0; i < m_vPSODescs.size(); i++)
	{
		if (m_vPSODescs[i] == *pPSODesc)
		{
			return m_vPSOs[i];
		}
	}


	ID3D12PipelineState* temp;
	DxAssert(pDevice->CreateGraphicsPipelineState(pPSODesc, IID_PPV_ARGS(&temp)), S_OK);

	m_vPSOs.push_back(temp);
	m_vPSODescs.push_back(*pPSODesc);
	
	return temp;
}

