#include "Shader.h"


Shader::Shader()
{
}

Shader::~Shader()
{
}

D3D12_SHADER_BYTECODE Shader::CompileShader(LPCWSTR path, LPCSTR entrypoint, LPCSTR shadermodel)
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


void Shader::SetShader(D3D12_SHADER_BYTECODE shaderByteCode, LPCSTR shadermodel)
{
	char c = shadermodel[0];
	if (c == 'v')
	{
		m_vertexShader = shaderByteCode;
	}
	else if (c == 'p')
	{
		m_pixelShader = shaderByteCode;
	}
}

void Shader::AddSampler(D3D12_STATIC_SAMPLER_DESC descSampler)
{
	m_vSamplers.push_back(descSampler);
}

std::vector<D3D12_STATIC_SAMPLER_DESC> Shader::GetSamplers()
{
	return m_vSamplers;
}


D3D12_SHADER_BYTECODE Shader::GetVertexShaderByteCode()
{
	return m_vertexShader;
}

D3D12_SHADER_BYTECODE Shader::GetPixelShaderByteCode()
{
	return m_pixelShader;
}