#include "Object.h"



Object::Object()
{
	m_pMesh = nullptr;
	m_pTexture = nullptr;
}

Object::~Object()
{
	if (m_pMesh)
		delete m_pMesh;
	if (m_pTexture)
		delete m_pTexture;
}

void Object::SetMesh(Mesh * pMesh)
{
	m_pMesh = pMesh;
}

void Object::SetTexture(Texture * pTexture)
{
	m_pTexture = pTexture;
}

Mesh * Object::GetMesh()
{
	return m_pMesh;
}

Texture * Object::GetTexture()
{
	return m_pTexture;
}
