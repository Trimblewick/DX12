#include "Pipeline.h"



Pipeline::Pipeline()
{
}

Pipeline::~Pipeline()
{
	SAFE_RELEASE(m_pPSO);
	SAFE_RELEASE(m_pRS);
	m_vObjects.clear();
}

void Pipeline::AddObject(Object * pObject)
{
	m_vObjects.push_back(pObject);
}

bool Pipeline::DrawObjects(ID3D12GraphicsCommandList * pCL, Camera* pCamera, int iBackBufferIndex)
{
	if (!m_pRS || !m_pPSO || m_vObjects.size() == 0)
		return false;

	pCL->SetGraphicsRootSignature(m_pRS);
	pCL->SetPipelineState(m_pPSO);
	
	pCamera->BindCameraBuffer(0, pCL, iBackBufferIndex);

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

