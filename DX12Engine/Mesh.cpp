#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

D3D12_PRIMITIVE_TOPOLOGY Mesh::GetPrimitiveTopology()
{
	return m_primitiveTopology;
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView()
{
	return m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView()
{
	return m_indexBufferView;
}

UINT Mesh::GetNrOfIndices()
{
	return m_iNrOfIndices;
}
