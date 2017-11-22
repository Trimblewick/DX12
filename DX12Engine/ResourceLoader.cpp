#include "ResourceLoader.h"

bool ResourceLoader::IsANumber(std::string sStr)
{
	return !sStr.empty() && sStr.find_first_not_of("0123456789") == sStr.npos;
}

BYTE * ResourceLoader::ParseOBJ()
{
	return nullptr;
}

ID3DBlob * ResourceLoader::CompileShader(LPCWSTR filePath, LPCSTR entrypoint, LPCSTR shadermodel)
{
	ID3DBlob* shaderBlob; 
	
	DxAssert(D3DCompileFromFile(filePath, nullptr, nullptr, entrypoint, shadermodel,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &shaderBlob, nullptr), S_OK);

	return shaderBlob;
}

ResourceLoader::ResourceLoader()
{
}

ResourceLoader::~ResourceLoader()
{
}



Mesh * ResourceLoader::LoadMeshFromFile(std::string sFileName, Mesh::MeshLayout meshLayout, GPUbridge* pGPUbridge)
{
	Mesh* pMesh = new Mesh();
	ID3D12CommandAllocator* pCA = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ID3D12GraphicsCommandList* pCL = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, pCA);


	std::string sLine;
	std::ifstream infile(sFileName);

	std::vector<DirectX::XMFLOAT3>		vPosition;
	std::vector<DirectX::XMFLOAT2>		vTexcoords;
	std::vector<DirectX::XMFLOAT3>		vNormals;
	std::vector<Vertex>					vVertices;
	std::vector<VertexNormal>			vVerticesNormals;
	std::vector<VertexTexcoordNormal>	vVerticesTexcoordsNormals;

	D3D12_SUBRESOURCE_DATA initData = {};
	int nrOfVertices = 0;
	int iSize = 0;
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};

	ID3D12Resource* pVertexBuffer = nullptr;
	ID3D12Resource* pUpploadHeap = nullptr;
	ID3D12Fence* pFence = D3DClass::CreateFence();
	HANDLE fenceHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	bool once = true;
	

	while (std::getline(infile, sLine))
	{
		std::istringstream iss(sLine);
		std::string sWord;
		iss >> sWord;
		
		if (sWord == "o")
		{
			if (once)
				once = false;
			else
				break;
		}
		

		
		if (sWord == "v")
		{
			DirectX::XMFLOAT3 vert;

			iss >> sWord;
			vert.x = std::stof(sWord);
			iss >> sWord;
			vert.y = std::stof(sWord);
			iss >> sWord;
			vert.z = std::stof(sWord);

			vPosition.push_back(vert);
		}
		else if (sWord == "vn")
		{
			DirectX::XMFLOAT3 norm;

			iss >> sWord;
			norm.x = std::stof(sWord);
			iss >> sWord;
			norm.y = std::stof(sWord);
			iss >> sWord;
			norm.z = std::stof(sWord);

			vNormals.push_back(norm);
		}
		else if (sWord == "vt")
		{
			//uvs....
		}
		else if (sWord == "f")
		{
			std::istringstream faceStream(sWord);
			std::string sSeparatedString;

			/*
			if (meshLayout == Mesh::MeshLayout::UNKNOWN)
			{
				std::string sTemp = sWord;
				iss >> sWord;
				std::getline(faceStream, sSeparatedString, '/');
				if (IsANumber(sSeparatedString))
				{
					std::getline(faceStream, sSeparatedString, '/');
					if (IsANumber(sSeparatedString))
					{
						std::getline(faceStream, sSeparatedString, '/');
						if (IsANumber(sSeparatedString))
						{
							//parse vert tex and normals
							meshLayout = Mesh::MeshLayout::VERTEXTEXCOORDNORMAL;
						}
					}
					else
					{
						std::getline(faceStream, sSeparatedString, '/');
						if (IsANumber(sSeparatedString))
						{
							//parse vert and normals
							meshLayout = Mesh::MeshLayout::VERTEXNORMAL;
						}
						else
						{
							//parse verts only
							meshLayout == Mesh::MeshLayout::VERTEX;
						}
					}
				}
			}*/
			while (iss >> sWord)
			{
				switch(meshLayout)
				{
				case Mesh::MeshLayout::VERTEX:
				{
					if (IsANumber(sWord))
					{
						Vertex v;
						v.position = vPosition[std::stoi(sWord) + 1];
						vVertices.push_back(v);
					}
					else
					{
						int temp_stopper = 0;
					}
					break;
				}
				case Mesh::MeshLayout::VERTEXNORMAL:
				{
					VertexNormal v;
					sSeparatedString = sWord.substr(0, sWord.find("//"));
					if (IsANumber(sSeparatedString))
					{
						v.position = vPosition[std::stoi(sSeparatedString) - 1];
					}
					sSeparatedString = sWord.substr(sWord.find("//") + 2, sWord.length());
					if (IsANumber(sSeparatedString))
					{
						v.normal = vNormals[std::stoi(sSeparatedString) - 1];
					}
					vVerticesNormals.push_back(v);
					break;
				}
				case Mesh::MeshLayout::VERTEXTEXCOORDNORMAL:
				{

				}
				}
				
			}
		}
	}
	
	switch (meshLayout)
	{
	case Mesh::MeshLayout::VERTEX:
	{
		nrOfVertices = vVertices.size();
		iSize = nrOfVertices * sizeof(Vertex);
		initData.pData = reinterpret_cast<BYTE*>(vVertices.data());
		initData.RowPitch = iSize;
		initData.SlicePitch = iSize;

		vertexBufferView.SizeInBytes = iSize;
		vertexBufferView.StrideInBytes = sizeof(Vertex);

		break;
	}
	case Mesh::MeshLayout::VERTEXNORMAL:
	{
		nrOfVertices = vVerticesNormals.size();
		iSize = nrOfVertices * sizeof(VertexNormal);
		initData.pData = reinterpret_cast<BYTE*>(vVerticesNormals.data());
		initData.RowPitch = iSize;
		initData.SlicePitch = iSize;

		vertexBufferView.SizeInBytes = iSize;
		vertexBufferView.StrideInBytes = sizeof(VertexNormal);

		

		break;
	}
	case Mesh::MeshLayout::VERTEXTEXCOORDNORMAL:
	{
		break;
	}
	}

	pVertexBuffer = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, iSize, D3D12_RESOURCE_STATE_COPY_DEST, NULL);
	vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();

	pUpploadHeap = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_UPLOAD, iSize, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	UpdateSubresources(pCL, pVertexBuffer, pUpploadHeap, 0, 0, 1, &initData);

	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	DxAssert(pCL->Close(), S_OK);

	

	ID3D12CommandList* ppCLs[] = { pCL };
	pGPUbridge->ExecuteDecoupledCLs(1, ppCLs, pFence, 1);

	DxAssert(pFence->SetEventOnCompletion(1, fenceHandle), S_OK);
	WaitForSingleObject(fenceHandle, INFINITE);
	

	pMesh->SetPrimitiveTopology(primitiveTopology);
	pMesh->SetNrOfVertices(nrOfVertices);
	pMesh->SetVertexBufferAndView(pVertexBuffer, vertexBufferView);


	SAFE_RELEASE(pFence);
	SAFE_RELEASE(pUpploadHeap);
	SAFE_RELEASE(pCL);
	SAFE_RELEASE(pCA);
	

	return pMesh;
}

Texture * ResourceLoader::LoadTextureFromFile()
{
	return nullptr;
}

Shader * ResourceLoader::CreateShader(LPCWSTR vsFilePath, LPCWSTR psFilePath)
{
	Shader* pShader = new Shader();
	pShader->SetVertexShader(CompileShader(vsFilePath, "main", "vs_5_1"));
	pShader->SetPixelShader(CompileShader(psFilePath, "main", "ps_5_1"));
	return pShader;
}
