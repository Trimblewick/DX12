#include "ResourceLoader.h"

bool ResourceLoader::IsANumber(std::string sStr)
{
	return !sStr.empty() && sStr.find_first_not_of("0123456789") == sStr.npos;
}

BYTE * ResourceLoader::ParseOBJ()
{
	return nullptr;
}

ResourceLoader::ResourceLoader()
{
}

ResourceLoader::~ResourceLoader()
{
}



Mesh * ResourceLoader::LoadMeshFromFile(std::string sFileName, Mesh::MeshLayout meshLayout)
{
	Mesh* pMesh = new Mesh();

	std::string sLine;
	std::ifstream infile(sFileName);
	std::vector<DirectX::XMFLOAT3>		vertices;
	std::vector<DirectX::XMFLOAT2>		texcoords;
	std::vector<DirectX::XMFLOAT3>		normals;
	std::vector<Vertex>					vertVector;
	std::vector<VertexNormal>			vertNormalVector;
	std::vector<VertexTexcoordNormal>	vertTexcoordNormalVector;

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

			vertices.push_back(vert);
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

			normals.push_back(norm);
		}
		else if (sWord == "vt")
		{
			//uvs....
		}
		else if (sWord == "f")
		{
			std::istringstream faceStream(sWord);
			std::string sSeparatedString;


			/*if (meshLayout == MeshLayout::UNKNOWN)
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
							meshLayout = MeshLayout::VERTEXTEXCOORDNORMAL;
						}
					}
					else
					{
						std::getline(faceStream, sSeparatedString, '/');
						if (IsANumber(sSeparatedString))
						{
							//parse vert and normals
							meshLayout = MeshLayout::VERTEXNORMAL;
						}
						else
						{
							//parse verts only
							meshLayout == MeshLayout::VERTEX;
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
							v.position = vertices[std::stoi(sWord) + 1];
							vertVector.push_back(v);
						}
						else
						{
							assert(1 == 0);//temp
						}
						break;
					}
					case Mesh::MeshLayout::VERTEXNORMAL:
					{
						VertexNormal v;
						sSeparatedString = sWord.substr(0, sWord.find("//"));
						if (IsANumber(sSeparatedString))
						{
							v.position = vertices[std::stoi(sSeparatedString) - 1];
						}
						sSeparatedString = sWord.substr(sWord.find("//") + 2, sWord.length());
						if (IsANumber(sSeparatedString))
						{
							v.normal = normals[std::stoi(sSeparatedString) - 1];
						}
						vertNormalVector.push_back(v);
						break;
					}
					case Mesh::MeshLayout::VERTEXTEXCOORDNORMAL:
					{

					}
				}
				
			}
		}
	}

	int stopper = 0;


	return pMesh;
}

Texture * ResourceLoader::LoadTextureFromFile()
{
	return nullptr;
}
