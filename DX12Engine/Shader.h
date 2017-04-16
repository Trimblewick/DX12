#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <string>
#include "Common.h"

class Shader
{
public:
	Shader();
	~Shader();

	D3D12_SHADER_BYTECODE CompileShader(LPCWSTR path, LPCSTR entrypoint, LPCSTR shadermodel);

private:

};

