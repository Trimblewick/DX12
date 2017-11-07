#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class Object
{
public:
	Object();
	~Object();

	void						SetMesh(Mesh* pMesh);
	void						SetTexture(Texture* pTexture);
	void						SetShader(Shader* pShader);

	Mesh*						GetMesh();
	Texture*					GetTexture();
	Shader*						GetShader();

	void						InitializeObject(DXGI_SWAP_CHAIN_DESC tempSwapChainDesc);

	ID3D12PipelineState*		GetPSO();
	ID3D12RootSignature*		GetRS();

private:
	Mesh*						m_pMesh;
	Texture*					m_pTexture;
	Shader*						m_pShader;
	
	ID3D12PipelineState*		m_pPSO;
	ID3D12RootSignature*		m_pRootSignature;
};
