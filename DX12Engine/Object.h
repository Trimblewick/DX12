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
	

	Mesh*						GetMesh();
	Texture*					GetTexture();
	
private:
	Mesh*						m_pMesh;
	Texture*					m_pTexture;
	
};
