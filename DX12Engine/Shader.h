#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include "Common.h"

class Shader
{
public:
	Shader();
	~Shader();

	static D3D12_SHADER_BYTECODE				CompileShader(LPCWSTR path, LPCSTR entrypoint, LPCSTR shadermodel, D3D12_SHADER_BYTECODE& sb);

	void										SetVertexShader(ID3DBlob* pVSblob);
	void										SetPixelShader(ID3DBlob* pPSblob);

	void										SetComputeShader(ID3DBlob* pCSblob);

	void										SetInputLayout(D3D12_INPUT_LAYOUT_DESC* desc);
	void										SetSampleDesc(DXGI_SAMPLE_DESC desc);
	
	bool										AddRootParameter(D3D12_ROOT_PARAMETER rootParameter);
	void										AddSampler(D3D12_STATIC_SAMPLER_DESC descSampler);


	D3D12_INPUT_LAYOUT_DESC*					GetInputLayout();

	D3D12_SHADER_BYTECODE						GetVertexShaderByteCode();
	D3D12_SHADER_BYTECODE						GetPixelShaderByteCode();
	D3D12_SHADER_BYTECODE						GetComputeShaderByteCode();

	DXGI_SAMPLE_DESC							GetSampleDesc();

	std::vector<D3D12_ROOT_PARAMETER>			GetRootParameters();
	int											GetNumRootParameters();
	D3D12_ROOT_PARAMETER*						GetRootParameterData();
	std::vector<D3D12_STATIC_SAMPLER_DESC>		GetSamplers();
	int											GetNumStaticSamplers();
	D3D12_STATIC_SAMPLER_DESC*					GetStaticSamplerData();

	bool										HasInputLayout();
	bool										HasVS();
	bool										HasPS();
	bool										HasSampleDesc();

	bool										HasCS();
private:
	D3D12_INPUT_LAYOUT_DESC*					m_inputLayoutDesc;
	DXGI_SAMPLE_DESC							m_sampleDesc;

	bool										m_bHasInputLayout;
	bool										m_bHasVS;
	bool										m_bHasPS;
	bool										m_bHasSampleDesc;

	bool										m_bHasCS;

	ID3DBlob*									m_pVSblob;
	ID3DBlob*									m_pPSblob;
	ID3DBlob*									m_pCSblob;

	std::vector<D3D12_ROOT_PARAMETER>			m_vRootParameters;
	std::vector<D3D12_STATIC_SAMPLER_DESC>		m_vSamplers;
};

