#pragma once

#include "D3dClass.h"
#include "Mesh.h"
#include "Texture.h"
#include <fstream>
#include <sstream>

class ResourceLoader
{
private:
	bool			IsANumber(std::string sStr);
	BYTE*			ParseOBJ();

public:
	ResourceLoader();
	~ResourceLoader();
	//Mesh
	
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
	struct VertexNormal
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct VertexTexcoordNormal
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
		DirectX::XMFLOAT3 normal;
	};

	Mesh*						LoadMeshFromFile(std::string sFileName, Mesh::MeshLayout meshLayout);
	Texture*					LoadTextureFromFile();


private:

};

