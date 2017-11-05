#pragma once
#include "D3dClass.h"

class Mesh
{

public:
	Mesh();
	~Mesh();
	enum MeshLayout
	{
		UNKNOWN = 0,
		VERTEX = 1,
		VERTEXNORMAL = 2,
		VERTEXTEXCOORDNORMAL = 3
	};

	void									SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);
	void									SetVertexBufferAndView(ID3D12Resource* pVertexBuffer, D3D12_VERTEX_BUFFER_VIEW vertexBufferView);
	void									SetNrOfVertices(UINT iNrOfVertices);

	
	D3D12_PRIMITIVE_TOPOLOGY				GetPrimitiveTopology();
	D3D12_VERTEX_BUFFER_VIEW				GetVertexBufferView();
	UINT									GetNrOfVertices();
	//D3D12_INDEX_BUFFER_VIEW					GetIndexBufferView();
	//UINT									GetNrOfIndices();
	
private:
	D3D12_PRIMITIVE_TOPOLOGY				m_primitiveTopology;

	ID3D12Resource*							m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW				m_vertexBufferView;

	UINT									m_iNrOfVertices;
	/*
	ID3D12Resource*							m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW					m_indexBufferView;

	UINT									m_iNrOfIndices;
	*/
};

