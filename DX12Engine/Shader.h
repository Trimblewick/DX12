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

	static D3D12_SHADER_BYTECODE				CompileShader(LPCWSTR path, LPCSTR entrypoint, LPCSTR shadermodel);

	void										SetShader(D3D12_SHADER_BYTECODE shaderByteCode, LPCSTR shadermodel);//this seems pretty bad
	void										AddSampler(D3D12_STATIC_SAMPLER_DESC descSampler);

	D3D12_SHADER_BYTECODE						GetVertexShaderByteCode();
	D3D12_SHADER_BYTECODE						GetPixelShaderByteCode();

	std::vector<D3D12_STATIC_SAMPLER_DESC>		GetSamplers();

private:
	D3D12_SHADER_BYTECODE						m_vertexShader;
	D3D12_SHADER_BYTECODE						m_pixelShader;

	std::vector<D3D12_STATIC_SAMPLER_DESC>		m_vSamplers;
};

