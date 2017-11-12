#include "Pipeline.h"



Pipeline::Pipeline()
{
}

Pipeline::~Pipeline()
{
}

void Pipeline::AddObject(Object * pObject)
{
	m_vObjects.push_back(pObject);
}

void Pipeline::DrawObjects(ID3D12GraphicsCommandList * pCL)
{
	//pCL->SetGraphicsRootSignature(m_pRS);
	//pCL->SetPipelineState(m_pPSO);
	
	for (Object* pObject : m_vObjects)
	{
		pCL->IASetPrimitiveTopology(pObject->GetMesh()->GetPrimitiveTopology());
		pCL->IASetVertexBuffers(0, 1, &pObject->GetMesh()->GetVertexBufferView());
		pCL->DrawInstanced(pObject->GetMesh()->GetNrOfVertices(), 1, 0, 0);
	}


}

void Pipeline::SetRS(ID3D12RootSignature * pRS)
{
	m_pRS = pRS;
}

void Pipeline::SetPSO(ID3D12PipelineState * pPSO)
{
	m_pPSO = pPSO;
}

