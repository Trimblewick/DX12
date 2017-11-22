#include "Object.h"



Object::Object()
{
	m_pMesh = nullptr;
	m_pTexture = nullptr;
}

Object::~Object()
{
}

void Object::SetMesh(Mesh * pMesh)
{
	m_pMesh = pMesh;
}

void Object::SetTexture(Texture * pTexture)
{
	m_pTexture = pTexture;
}

Mesh * Object::GetMesh()
{
	return m_pMesh;
}

Texture * Object::GetTexture()
{
	return m_pTexture;
}
/*
void Object::InitializeObject(DXGI_SWAP_CHAIN_DESC tempSwapChainDesc)
{
	
	D3D12_INPUT_ELEMENT_DESC inputLayoutElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	D3D12_INPUT_LAYOUT_DESC					descInputLayout = {};
	std::vector<CD3DX12_ROOT_PARAMETER>		rootParameters;
	CD3DX12_ROOT_SIGNATURE_DESC				descRS;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC		descPSO = {};

	descInputLayout.NumElements = sizeof(inputLayoutElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	descInputLayout.pInputElementDescs = inputLayoutElementDesc;




	//create rootsignature
	std::vector<D3D12_STATIC_SAMPLER_DESC> vSamplers = m_pShader->GetSamplers();
	descRS.Init(rootParameters.size(), rootParameters.data(), vSamplers.size(), vSamplers.data());
	m_pRootSignature = D3DClass::CreateRS(&descRS);


	descPSO.InputLayout = descInputLayout;
	descPSO.pRootSignature = m_pRootSignature;
	descPSO.VS = m_pShader->GetVertexShaderByteCode();
	descPSO.PS = m_pShader->GetPixelShaderByteCode();
	descPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPSO.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	descPSO.SampleDesc = tempSwapChainDesc.SampleDesc;
	descPSO.SampleMask = 0xffffffff;
	descPSO.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	descPSO.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	descPSO.NumRenderTargets = 1;
	//psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC();
	descPSO.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	m_pPSO = D3DClass::CreateGraphicsPSO(&descPSO);

}
*/