#pragma once

#include <d3d12.h>
#include <vector>
#include "Object.h"

class Pipeline
{
public:
	Pipeline();
	~Pipeline();


	void										AddObject(Object* pObject);//make sure the object can be rendered with this pipeline
	void										DrawObjects(ID3D12GraphicsCommandList* pCL);
	void										SetRS(ID3D12RootSignature* pRS);
	void										SetPSO(ID3D12PipelineState* pPSO);


private:
	ID3D12RootSignature*						m_pRS;
	ID3D12PipelineState*						m_pPSO;

	std::vector<Object*>						m_vObjects;
	
};
