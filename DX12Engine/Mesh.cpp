#include "Mesh.h"

Mesh::Mesh()
{
	m_pVertexBuffer = nullptr;
	m_iNrOfVertices = 0;
}

Mesh::~Mesh()
{
}

void Mesh::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_primitiveTopology = primitiveTopology;
}

void Mesh::SetVertexBufferAndView(ID3D12Resource * pVertexBuffer, D3D12_VERTEX_BUFFER_VIEW vertexBufferView)
{
	m_pVertexBuffer = pVertexBuffer;
	m_vertexBufferView = vertexBufferView;
} 

void Mesh::SetNrOfVertices(UINT iNrOfVertices)
{
	m_iNrOfVertices = iNrOfVertices;
}

D3D12_PRIMITIVE_TOPOLOGY Mesh::GetPrimitiveTopology()
{
	return m_primitiveTopology;
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView()
{
	return m_vertexBufferView;
}

UINT Mesh::GetNrOfVertices()
{
	return m_iNrOfVertices;
}
