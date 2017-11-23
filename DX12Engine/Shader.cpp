#include "Shader.h"

#define flase = false;

Shader::Shader()
{
	m_pVSblob = nullptr;
	m_pPSblob = nullptr;

	m_bHasInputLayout = false;
	m_bHasVS = false;
	m_bHasPS = false;
	m_bHasSampleDesc = false;
}

Shader::~Shader()
{
	SAFE_RELEASE(m_pVSblob);
	SAFE_RELEASE(m_pPSblob);
}

D3D12_SHADER_BYTECODE Shader::CompileShader(LPCWSTR path, LPCSTR entrypoint, LPCSTR shadermodel, D3D12_SHADER_BYTECODE& sb)
{
	ID3DBlob* shaderBlob; // d3d blob for holding shader bytecode
	ID3DBlob* errorBlob; // a buffer holding the error data if any
	DxAssert(D3DCompileFromFile(path, nullptr, nullptr, entrypoint, shadermodel, 
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &shaderBlob, &errorBlob), S_OK);
	
	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE shaderBytecode = {};
	shaderBytecode.BytecodeLength = shaderBlob->GetBufferSize();
	shaderBytecode.pShaderBytecode = shaderBlob->GetBufferPointer();

	SAFE_RELEASE(shaderBlob);

	return shaderBytecode;
}

void Shader::SetVertexShader(ID3DBlob* pVSblob)
{
	m_pVSblob = pVSblob;
	m_bHasVS = true;
}

void Shader::SetPixelShader(ID3DBlob* pPSblob)
{
	m_pPSblob = pPSblob;
	m_bHasPS = true;
}

void Shader::SetInputLayout(D3D12_INPUT_LAYOUT_DESC* desc)
{
	m_inputLayoutDesc = desc;
	m_bHasInputLayout = true;
}

void Shader::SetSampleDesc(DXGI_SAMPLE_DESC desc)
{
	m_sampleDesc = desc;
	m_bHasSampleDesc = true;
}

bool Shader::AddRootParameter(D3D12_ROOT_PARAMETER rootParameter)
{
	bool bPass = true;

	for (D3D12_ROOT_PARAMETER rp : m_vRootParameters)
	{
		if (rootParameter.ParameterType == rp.ParameterType)
		{
			if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
				rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
				rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
			{
				if (rp.Descriptor.ShaderRegister == rootParameter.Descriptor.ShaderRegister)
				{
					bPass = false;
				}
			}
			if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS)
			{
				if (rp.Constants.ShaderRegister == rootParameter.Constants.ShaderRegister)
				{
					bPass = false;
				}
			}
			if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				if (rp.DescriptorTable.pDescriptorRanges->BaseShaderRegister == rootParameter.DescriptorTable.pDescriptorRanges->BaseShaderRegister)
				{
					bPass = false;
				}
			}
		}
	}

	if (bPass)//if it doesn't bPass, then there already is a rootparameter in the pipeline that is assigned to the same register -> of the same type....
		m_vRootParameters.push_back(rootParameter);

	return bPass;
}

void Shader::AddSampler(D3D12_STATIC_SAMPLER_DESC descSampler)
{
	m_vSamplers.push_back(descSampler);
}

std::vector<D3D12_ROOT_PARAMETER> Shader::GetRootParameters()
{
	return m_vRootParameters;
}

D3D12_ROOT_PARAMETER * Shader::GetRootParameterData()
{
	return m_vRootParameters.data();
}

std::vector<D3D12_STATIC_SAMPLER_DESC> Shader::GetSamplers()
{
	return m_vSamplers;
}

bool Shader::HasInputLayout()
{
	return m_bHasInputLayout;
}

bool Shader::HasVS()
{
	return m_bHasVS;
}

bool Shader::HasPS()
{
	return m_bHasPS;
}

bool Shader::HasSampleDesc()
{
	return m_bHasSampleDesc;
}

D3D12_INPUT_LAYOUT_DESC* Shader::GetInputLayout()
{
	return m_inputLayoutDesc;
}

D3D12_SHADER_BYTECODE Shader::GetVertexShaderByteCode()
{
	return { m_pVSblob->GetBufferPointer(), m_pVSblob->GetBufferSize() };
}

D3D12_SHADER_BYTECODE Shader::GetPixelShaderByteCode()
{
	return { m_pPSblob->GetBufferPointer(), m_pPSblob->GetBufferSize() };
}

DXGI_SAMPLE_DESC Shader::GetSampleDesc()
{
	return m_sampleDesc;
}
