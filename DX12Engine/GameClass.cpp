#include "GameClass.h"


GameClass::GameClass()
{
}

GameClass::~GameClass()
{
	
}

struct Tri
{
	DirectX::XMFLOAT4 p1;
	DirectX::XMFLOAT4 p2;
	DirectX::XMFLOAT4 p3;
	DirectX::XMFLOAT4 color;
};

struct Light
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
};

Light* CreateLights()
{
	Light* pLights = new Light[3];

	pLights[0].position = DirectX::XMFLOAT4(1.5f, -1.5f, 1.5f, 1.0f);
	pLights[0].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pLights[1].position = DirectX::XMFLOAT4(3.0f, 3.0f, 3.0f, 1.0f);
	pLights[1].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pLights[2].position = DirectX::XMFLOAT4(-4.2f, 4.0f, -4.0f, 1.0f);
	pLights[2].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	return pLights;
}

Tri* GetBoxMesh()
{
	Tri* pMesh = new Tri[24];

	DirectX::XMFLOAT3 box1Pos = DirectX::XMFLOAT3(10, 10, 10);
	DirectX::XMFLOAT4 box1Color = DirectX::XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 box2Pos = DirectX::XMFLOAT3(50, 50, 50);

	//Small box
	//Back
	pMesh[0].p1.x = box1Pos.x;
	pMesh[0].p1.y = -box1Pos.y;
	pMesh[0].p1.z = box1Pos.z;
	pMesh[0].p1.w = 1.0f;

	pMesh[0].p2.x = box1Pos.x;
	pMesh[0].p2.y = box1Pos.y;
	pMesh[0].p2.z = box1Pos.z;
	pMesh[0].p2.w = 1.0f;

	pMesh[0].p3.x = -box1Pos.x;
	pMesh[0].p3.y = -box1Pos.y;
	pMesh[0].p3.z = box1Pos.z;
	pMesh[0].p3.w = 1.0f;

	pMesh[0].color = box1Color;

	pMesh[1].p1.x = box1Pos.x;
	pMesh[1].p1.y = box1Pos.y;
	pMesh[1].p1.z = box1Pos.z;
	pMesh[1].p1.w = 1.0f;

	pMesh[1].p2.x = -box1Pos.x;
	pMesh[1].p2.y = box1Pos.y;
	pMesh[1].p2.z = box1Pos.z;
	pMesh[1].p2.w = 1.0f;

	pMesh[1].p3.x = -box1Pos.x;
	pMesh[1].p3.y = -box1Pos.y;
	pMesh[1].p3.z = box1Pos.z;
	pMesh[1].p3.w = 1.0f;

	pMesh[1].color = box1Color;

	//Front?
	pMesh[2].p1.x = box1Pos.x;
	pMesh[2].p1.y = -box1Pos.y;
	pMesh[2].p1.z = -box1Pos.z;
	pMesh[2].p1.w = 1.0f;

	pMesh[2].p2.x = box1Pos.x;
	pMesh[2].p2.y = box1Pos.y;
	pMesh[2].p2.z = -box1Pos.z;
	pMesh[2].p2.w = 1.0f;

	pMesh[2].p3.x = -box1Pos.x;
	pMesh[2].p3.y = -box1Pos.y;
	pMesh[2].p3.z = -box1Pos.z;
	pMesh[2].p3.w = 1.0f;

	pMesh[2].color = box1Color;

	pMesh[3].p1.x = box1Pos.x;
	pMesh[3].p1.y = box1Pos.y;
	pMesh[3].p1.z = -box1Pos.z;
	pMesh[3].p1.w = 1.0f;

	pMesh[3].p2.x = -box1Pos.x;
	pMesh[3].p2.y = box1Pos.y;
	pMesh[3].p2.z = -box1Pos.z;
	pMesh[3].p2.w = 1.0f;

	pMesh[3].p3.x = -box1Pos.x;
	pMesh[3].p3.y = -box1Pos.y;
	pMesh[3].p3.z = -box1Pos.z;
	pMesh[3].p3.w = 1.0f;

	pMesh[3].color = box1Color;

	//Right
	pMesh[4].p1.x = -box1Pos.x;
	pMesh[4].p1.y = -box1Pos.y;
	pMesh[4].p1.z = -box1Pos.z;
	pMesh[4].p1.w = 1.0f;

	pMesh[4].p2.x = -box1Pos.x;
	pMesh[4].p2.y = box1Pos.y;
	pMesh[4].p2.z = -box1Pos.z;
	pMesh[4].p2.w = 1.0f;

	pMesh[4].p3.x = -box1Pos.x;
	pMesh[4].p3.y = -box1Pos.y;
	pMesh[4].p3.z = box1Pos.z;
	pMesh[4].p3.w = 1.0f;

	pMesh[4].color = box1Color;

	pMesh[5].p1.x = -box1Pos.x;
	pMesh[5].p1.y = -box1Pos.y;
	pMesh[5].p1.z = box1Pos.z;
	pMesh[5].p1.w = 1.0f;

	pMesh[5].p2.x = -box1Pos.x;
	pMesh[5].p2.y = box1Pos.y;
	pMesh[5].p2.z = -box1Pos.z;
	pMesh[5].p2.w = 1.0f;

	pMesh[5].p3.x = -box1Pos.x;
	pMesh[5].p3.y = box1Pos.y;
	pMesh[5].p3.z = box1Pos.z;
	pMesh[5].p3.w = 1.0f;

	pMesh[5].color = box1Color;

	//Right
	pMesh[6].p1.x = box1Pos.x;
	pMesh[6].p1.y = -box1Pos.y;
	pMesh[6].p1.z = -box1Pos.z;
	pMesh[6].p1.w = 1.0f;

	pMesh[6].p2.x = box1Pos.x;
	pMesh[6].p2.y = -box1Pos.y;
	pMesh[6].p2.z = box1Pos.z;
	pMesh[6].p2.w = 1.0f;
	
	pMesh[6].p3.x = box1Pos.x;
	pMesh[6].p3.y = box1Pos.y;
	pMesh[6].p3.z = -box1Pos.z;
	pMesh[6].p3.w = 1.0f;

	pMesh[6].color.x = 0.5f;
	pMesh[6].color.y = 1.0f;
	pMesh[6].color.z = 0.0f;
	pMesh[6].color.w = 1.0f;

	pMesh[7].p1.x = box1Pos.x;
	pMesh[7].p1.y = -box1Pos.y;
	pMesh[7].p1.z = box1Pos.z;
	pMesh[7].p1.w = 1.0f;

	pMesh[7].p2.x = box1Pos.x;
	pMesh[7].p2.y = box1Pos.y;
	pMesh[7].p2.z = box1Pos.z;
	pMesh[7].p2.w = 1.0f;

	pMesh[7].p3.x = box1Pos.x;
	pMesh[7].p3.y = box1Pos.y;
	pMesh[7].p3.z = -box1Pos.z;
	pMesh[7].p3.w = 1.0f;

	pMesh[7].color = box1Color;

	//Bottom
	pMesh[8].p1.x = -box1Pos.x;
	pMesh[8].p1.y = box1Pos.y;
	pMesh[8].p1.z = box1Pos.z;
	pMesh[8].p1.w = 1.0f;

	pMesh[8].p2.x = -box1Pos.x;
	pMesh[8].p2.y = box1Pos.y;
	pMesh[8].p2.z = -box1Pos.z;
	pMesh[8].p2.w = 1.0f;

	pMesh[8].p3.x = box1Pos.x;
	pMesh[8].p3.y = box1Pos.y;
	pMesh[8].p3.z = box1Pos.z;
	pMesh[8].p3.w = 1.0f;

	pMesh[8].color = box1Color;

	pMesh[9].p1.x = box1Pos.x;
	pMesh[9].p1.y = box1Pos.y;
	pMesh[9].p1.z = box1Pos.z;
	pMesh[9].p1.w = 1.0f;

	pMesh[9].p2.x = -box1Pos.x;
	pMesh[9].p2.y = box1Pos.y;
	pMesh[9].p2.z = -box1Pos.z;
	pMesh[9].p2.w = 1.0f;

	pMesh[9].p3.x = box1Pos.x;
	pMesh[9].p3.y = box1Pos.y;
	pMesh[9].p3.z = -box1Pos.z;
	pMesh[9].p3.w = 1.0f;

	pMesh[9].color = box1Color;

	//Bottom
	pMesh[10].p1.x = -box1Pos.x;
	pMesh[10].p1.y = -box1Pos.y;
	pMesh[10].p1.z = -box1Pos.z;
	pMesh[10].p1.w = 1.0f;

	pMesh[10].p2.x = -box1Pos.x;
	pMesh[10].p2.y = -box1Pos.y;
	pMesh[10].p2.z = box1Pos.z;
	pMesh[10].p2.w = 1.0f;

	pMesh[10].p3.x = box1Pos.x;
	pMesh[10].p3.y = -box1Pos.y;
	pMesh[10].p3.z = box1Pos.z;
	pMesh[10].p3.w = 1.0f;

	pMesh[10].color = box1Color;

	pMesh[11].p1.x = -box1Pos.x;
	pMesh[11].p1.y = -box1Pos.y;
	pMesh[11].p1.z = -box1Pos.z;
	pMesh[11].p1.w = 1.0f;

	pMesh[11].p2.x = box1Pos.x;
	pMesh[11].p2.y = -box1Pos.y;
	pMesh[11].p2.z = box1Pos.z;
	pMesh[11].p2.w = 1.0f;

	pMesh[11].p3.x = box1Pos.x;
	pMesh[11].p3.y = -box1Pos.y;
	pMesh[11].p3.z = -box1Pos.z;
	pMesh[11].p3.w = 1.0f;

	pMesh[11].color = box1Color;

	//Big box
	//Back
	pMesh[0 + 12].p1.x = box2Pos.x;
	pMesh[0 + 12].p1.y = -box2Pos.y;
	pMesh[0 + 12].p1.z = box2Pos.z;
	pMesh[0 + 12].p1.w = 1.0f;

	pMesh[0 + 12].p2.x = box2Pos.x;
	pMesh[0 + 12].p2.y = box2Pos.y;
	pMesh[0 + 12].p2.z = box2Pos.z;
	pMesh[0 + 12].p2.w = 1.0f;

	pMesh[0 + 12].p3.x = -box2Pos.x;
	pMesh[0 + 12].p3.y = -box2Pos.y;
	pMesh[0 + 12].p3.z = box2Pos.z;
	pMesh[0 + 12].p3.w = 1.0f;

	pMesh[0 + 12].color.x = 0.5f;
	pMesh[0 + 12].color.y = 0.0f;
	pMesh[0 + 12].color.z = 0.0f;
	pMesh[0 + 12].color.w = 1.0f;

	pMesh[1 + 12].p1.x = box2Pos.x;
	pMesh[1 + 12].p1.y = box2Pos.y;
	pMesh[1 + 12].p1.z = box2Pos.z;
	pMesh[1 + 12].p1.w = 1.0f;

	pMesh[1 + 12].p2.x = -box2Pos.x;
	pMesh[1 + 12].p2.y = box2Pos.y;
	pMesh[1 + 12].p2.z = box2Pos.z;
	pMesh[1 + 12].p2.w = 1.0f;

	pMesh[1 + 12].p3.x = -box2Pos.x;
	pMesh[1 + 12].p3.y = -box2Pos.y;
	pMesh[1 + 12].p3.z = box2Pos.z;
	pMesh[1 + 12].p3.w = 1.0f;

	pMesh[1 + 12].color.x = 0.5f;
	pMesh[1 + 12].color.y = 0.0f;
	pMesh[1 + 12].color.z = 0.0f;
	pMesh[1 + 12].color.w = 1.0f;

	//Front?
	pMesh[2 + 12].p1.x = box2Pos.x;
	pMesh[2 + 12].p1.y = -box2Pos.y;
	pMesh[2 + 12].p1.z = -box2Pos.z;
	pMesh[2 + 12].p1.w = 1.0f;

	pMesh[2 + 12].p2.x = box2Pos.x;
	pMesh[2 + 12].p2.y = box2Pos.y;
	pMesh[2 + 12].p2.z = -box2Pos.z;
	pMesh[2 + 12].p2.w = 1.0f;

	pMesh[2 + 12].p3.x = -box2Pos.x;
	pMesh[2 + 12].p3.y = -box2Pos.y;
	pMesh[2 + 12].p3.z = -box2Pos.z;
	pMesh[2 + 12].p3.w = 1.0f;

	pMesh[2 + 12].color.x = 0.5f;
	pMesh[2 + 12].color.y = 0.0f;
	pMesh[2 + 12].color.z = 0.0f;
	pMesh[2 + 12].color.w = 1.0f;

	pMesh[3 + 12].p1.x = box2Pos.x;
	pMesh[3 + 12].p1.y = box2Pos.y;
	pMesh[3 + 12].p1.z = -box2Pos.z;
	pMesh[3 + 12].p1.w = 1.0f;

	pMesh[3 + 12].p2.x = -box2Pos.x;
	pMesh[3 + 12].p2.y = box2Pos.y;
	pMesh[3 + 12].p2.z = -box2Pos.z;
	pMesh[3 + 12].p2.w = 1.0f;

	pMesh[3 + 12].p3.x = -box2Pos.x;
	pMesh[3 + 12].p3.y = -box2Pos.y;
	pMesh[3 + 12].p3.z = -box2Pos.z;
	pMesh[3 + 12].p3.w = 1.0f;

	pMesh[3 + 12].color.x = 0.5f;
	pMesh[3 + 12].color.y = 0.0f;
	pMesh[3 + 12].color.z = 0.0f;
	pMesh[3 + 12].color.w = 1.0f;

	//Right
	pMesh[4 + 12].p1.x = -box2Pos.x;
	pMesh[4 + 12].p1.y = -box2Pos.y;
	pMesh[4 + 12].p1.z = -box2Pos.z;
	pMesh[4 + 12].p1.w = 1.0f;

	pMesh[4 + 12].p2.x = -box2Pos.x;
	pMesh[4 + 12].p2.y = box2Pos.y;
	pMesh[4 + 12].p2.z = -box2Pos.z;
	pMesh[4 + 12].p2.w = 1.0f;

	pMesh[4 + 12].p3.x = -box2Pos.x;
	pMesh[4 + 12].p3.y = -box2Pos.y;
	pMesh[4 + 12].p3.z = box2Pos.z;
	pMesh[4 + 12].p3.w = 1.0f;

	pMesh[4 + 12].color.x = 0.5f;
	pMesh[4 + 12].color.y = 0.0f;
	pMesh[4 + 12].color.z = 0.0f;
	pMesh[4 + 12].color.w = 1.0f;

	pMesh[5 + 12].p1.x = -box2Pos.x;
	pMesh[5 + 12].p1.y = -box2Pos.y;
	pMesh[5 + 12].p1.z = box2Pos.z;
	pMesh[5 + 12].p1.w = 1.0f;

	pMesh[5 + 12].p2.x = -box2Pos.x;
	pMesh[5 + 12].p2.y = box2Pos.y;
	pMesh[5 + 12].p2.z = -box2Pos.z;
	pMesh[5 + 12].p2.w = 1.0f;

	pMesh[5 + 12].p3.x = -box2Pos.x;
	pMesh[5 + 12].p3.y = box2Pos.y;
	pMesh[5 + 12].p3.z = box2Pos.z;
	pMesh[5 + 12].p3.w = 1.0f;

	pMesh[5 + 12].color.x = 0.5f;
	pMesh[5 + 12].color.y = 0.0f;
	pMesh[5 + 12].color.z = 0.0f;
	pMesh[5 + 12].color.w = 1.0f;

	//Right
	pMesh[6 + 12].p1.x = box2Pos.x;
	pMesh[6 + 12].p1.y = box2Pos.y;
	pMesh[6 + 12].p1.z = -box2Pos.z;
	pMesh[6 + 12].p1.w = 1.0f;

	pMesh[6 + 12].p2.x = box2Pos.x;
	pMesh[6 + 12].p2.y = -box2Pos.y;
	pMesh[6 + 12].p2.z = -box2Pos.z;
	pMesh[6 + 12].p2.w = 1.0f;

	pMesh[6 + 12].p3.x = box2Pos.x;
	pMesh[6 + 12].p3.y = -box2Pos.y;
	pMesh[6 + 12].p3.z = box2Pos.z;
	pMesh[6 + 12].p3.w = 1.0f;

	pMesh[6 + 12].color.x = 0.5f;
	pMesh[6 + 12].color.y = 0.0f;
	pMesh[6 + 12].color.z = 0.0f;
	pMesh[6 + 12].color.w = 1.0f;

	pMesh[7 + 12].p1.x = box2Pos.x;
	pMesh[7 + 12].p1.y = -box2Pos.y;
	pMesh[7 + 12].p1.z = box2Pos.z;
	pMesh[7 + 12].p1.w = 1.0f;

	pMesh[7 + 12].p2.x = box2Pos.x;
	pMesh[7 + 12].p2.y = box2Pos.y;
	pMesh[7 + 12].p2.z = box2Pos.z;
	pMesh[7 + 12].p2.w = 1.0f;

	pMesh[7 + 12].p3.x = box2Pos.x;
	pMesh[7 + 12].p3.y = box2Pos.y;
	pMesh[7 + 12].p3.z = -box2Pos.z;
	pMesh[7 + 12].p3.w = 1.0f;

	pMesh[7 + 12].color.x = 0.5f;
	pMesh[7 + 12].color.y = 0.0f;
	pMesh[7 + 12].color.z = 0.0f;
	pMesh[7 + 12].color.w = 1.0f;

	//Bottom
	pMesh[8 + 12].p1.x = -box2Pos.x;
	pMesh[8 + 12].p1.y = box2Pos.y;
	pMesh[8 + 12].p1.z = box2Pos.z;
	pMesh[8 + 12].p1.w = 1.0f;

	pMesh[8 + 12].p2.x = -box2Pos.x;
	pMesh[8 + 12].p2.y = box2Pos.y;
	pMesh[8 + 12].p2.z = -box2Pos.z;
	pMesh[8 + 12].p2.w = 1.0f;

	pMesh[8 + 12].p3.x = box2Pos.x;
	pMesh[8 + 12].p3.y = box2Pos.y;
	pMesh[8 + 12].p3.z = box2Pos.z;
	pMesh[8 + 12].p3.w = 1.0f;

	pMesh[8 + 12].color.x = 0.5f;
	pMesh[8 + 12].color.y = 0.0f;
	pMesh[8 + 12].color.z = 0.0f;
	pMesh[8 + 12].color.w = 1.0f;

	pMesh[9 + 12].p1.x = -box2Pos.x;
	pMesh[9 + 12].p1.y = box2Pos.y;
	pMesh[9 + 12].p1.z = -box2Pos.z;
	pMesh[9 + 12].p1.w = 1.0f;

	pMesh[9 + 12].p2.x = box2Pos.x;
	pMesh[9 + 12].p2.y = box2Pos.y;
	pMesh[9 + 12].p2.z = -box2Pos.z;
	pMesh[9 + 12].p2.w = 1.0f;

	pMesh[9 + 12].p3.x = box2Pos.x;
	pMesh[9 + 12].p3.y = box2Pos.y;
	pMesh[9 + 12].p3.z = box2Pos.z;
	pMesh[9 + 12].p3.w = 1.0f;

	pMesh[9 + 12].color.x = 0.5f;
	pMesh[9 + 12].color.y = 0.0f;
	pMesh[9 + 12].color.z = 0.0f;
	pMesh[9 + 12].color.w = 1.0f;

	//Bottom
	pMesh[10 + 12].p1.x = -box2Pos.x;
	pMesh[10 + 12].p1.y = -box2Pos.y;
	pMesh[10 + 12].p1.z = -box2Pos.z;
	pMesh[10 + 12].p1.w = 1.0f;

	pMesh[10 + 12].p2.x = -box2Pos.x;
	pMesh[10 + 12].p2.y = -box2Pos.y;
	pMesh[10 + 12].p2.z = box2Pos.z;
	pMesh[10 + 12].p2.w = 1.0f;

	pMesh[10 + 12].p3.x = box2Pos.x;
	pMesh[10 + 12].p3.y = -box2Pos.y;
	pMesh[10 + 12].p3.z = box2Pos.z;
	pMesh[10 + 12].p3.w = 1.0f;

	pMesh[10 + 12].color.x = 0.5f;
	pMesh[10 + 12].color.y = 0.0f;
	pMesh[10 + 12].color.z = 0.0f;
	pMesh[10 + 12].color.w = 1.0f;

	pMesh[11 + 12].p1.x = box2Pos.x;
	pMesh[11 + 12].p1.y = -box2Pos.y;
	pMesh[11 + 12].p1.z = box2Pos.z;
	pMesh[11 + 12].p1.w = 1.0f;

	pMesh[11 + 12].p2.x = box2Pos.x;
	pMesh[11 + 12].p2.y = -box2Pos.y;
	pMesh[11 + 12].p2.z = -box2Pos.z;
	pMesh[11 + 12].p2.w = 1.0f;

	pMesh[11 + 12].p3.x = -box2Pos.x;
	pMesh[11 + 12].p3.y = -box2Pos.y;
	pMesh[11 + 12].p3.z = -box2Pos.z;
	pMesh[11 + 12].p3.w = 1.0f;

	pMesh[11 + 12].color.x = 0.5f;
	pMesh[11 + 12].color.y = 0.0f;
	pMesh[11 + 12].color.z = 0.0f;
	pMesh[11 + 12].color.w = 1.0f;

	return pMesh;
}

Tri* GenerateTriangles(int iNrOfTriangles)
{
	Tri* triangles = new Tri[iNrOfTriangles];
	for (int i = 0; i < iNrOfTriangles; ++i)
	{
		Tri temp;

		temp.p1.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.0f;
		temp.p2.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.p3.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f;
		temp.p1.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p2.y = 1.0f;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p3.y = 0.0f;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p1.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 2.0f;
		temp.p2.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 2.0f;
		temp.p3.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 4.0f - 2.0f;

		temp.p1.w = 1.0f;// static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p2.w = 1.0f;// static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.p3.w = 1.0f;// static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
		temp.color.x = 0;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.color.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.color.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		temp.color.w = 1.0f;
		
		triangles[i] = temp;
	}
	return triangles;
}

bool GameClass::Initialize()
{
	m_pMainCamera = new Camera(DirectX::XMFLOAT3(15.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pRenderer = new DeferredRenderer();
	m_pGPUbridge = new GPUbridge();
	m_pResourceLoader = new ResourceLoader();


	m_pRenderer->Initialize(m_pGPUbridge->GetCQ());

	//LOAD MESH
	//m_pObject = new Object();
	//m_pObject->SetMesh(m_pResourceLoader->LoadMeshFromFile("../Resources/Teapot/teapot_n_glass.obj", Mesh::MeshLayout::VERTEXNORMAL, m_pGPUbridge));

	m_pDHlab = D3DClass::CreateDH(5, D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	int iOffsetSize = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	
	desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	


	for (int i = 0; i < g_iBackBufferCount; ++i)
	{
		m_ppUAVTargets[i] = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, WindowClass::GetWidth(), WindowClass::GetHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS), D3D12_RESOURCE_STATE_GENERIC_READ);

		D3DClass::GetDevice()->CreateUnorderedAccessView(m_ppUAVTargets[i], nullptr, nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * i));
		//D3DClass::GetDevice()->CreateShaderResourceView(m_ppUAVTargets[i], nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * i));
	}
	
	iNrOfTriangles = 24;
	iNrOfLights = 3;
	Tri* pTiranglesData = GetBoxMesh();//GenerateTriangles(iNrOfTriangles);
	Light* pLights = CreateLights();

	ID3D12CommandAllocator* pCA = D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE_DIRECT);
	ID3D12GraphicsCommandList* pCL = D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE_DIRECT, pCA);

	ID3D12Resource* pTriMatUploadHeap = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_UPLOAD, sizeof(Tri) * iNrOfTriangles, D3D12_RESOURCE_STATE_GENERIC_READ, L"TriMatrixUploadHeap");
	ID3D12Resource* pLightMatUploadHeap = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_UPLOAD, sizeof(Light) * iNrOfLights, D3D12_RESOURCE_STATE_GENERIC_READ, L"LightUploadHeap");
	m_pTriangleMatrices = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, sizeof(Tri) * iNrOfTriangles, D3D12_RESOURCE_STATE_COPY_DEST, L"TriMatrixHeap");
	m_pLightHeap = D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, sizeof(Light) * iNrOfLights, D3D12_RESOURCE_STATE_COPY_DEST, L"LightHeap");
	
	D3D12_SUBRESOURCE_DATA triInitData = {};
	triInitData.pData = reinterpret_cast<BYTE*>(pTiranglesData);
	triInitData.SlicePitch = 1;// sizeof(Tri) * iNrOfTriangles;
	triInitData.RowPitch = sizeof(Tri) * iNrOfTriangles;

	D3D12_SUBRESOURCE_DATA lightsInitData = {};
	lightsInitData.pData = reinterpret_cast<BYTE*>(pLights);
	lightsInitData.SlicePitch = 1;// sizeof(Light) * iNrOfLights;
	lightsInitData.RowPitch = sizeof(Light) * iNrOfLights;

	UpdateSubresources(pCL, m_pTriangleMatrices, pTriMatUploadHeap, 0, 0, 1, &triInitData);
	UpdateSubresources(pCL, m_pLightHeap, pLightMatUploadHeap, 0, 0, 1, &lightsInitData);

	D3D12_RESOURCE_BARRIER barriers[] = { CD3DX12_RESOURCE_BARRIER().Transition(m_pTriangleMatrices, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ),
		CD3DX12_RESOURCE_BARRIER().Transition(m_pLightHeap, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ) };
	pCL->ResourceBarrier(2, barriers);

	ID3D12Fence* uploadFence = D3DClass::CreateFence();
	HANDLE uploadFenceHandle = CreateEvent(NULL, NULL, NULL, NULL);
	pCL->Close();
	ID3D12CommandList* ppCL[] = { pCL };

	m_pGPUbridge->ExecuteDecoupledCLs(1, ppCL, uploadFence, 1);
	uploadFence->SetEventOnCompletion(1, uploadFenceHandle);
	WaitForSingleObject(uploadFenceHandle, INFINITE);

	free(pTiranglesData);
	SAFE_RELEASE(pCL);
	SAFE_RELEASE(pCA);
	SAFE_RELEASE(pTriMatUploadHeap);

	SAFE_RELEASE(pLightMatUploadHeap);

	D3D12_BUFFER_SRV triSrvBufferDesc = {};
	triSrvBufferDesc.NumElements = iNrOfTriangles;
	triSrvBufferDesc.StructureByteStride = sizeof(Tri);
	triSrvBufferDesc.FirstElement = 0;
	triSrvBufferDesc.Flags = D3D12_BUFFER_SRV_FLAG_NONE;


	D3D12_SHADER_RESOURCE_VIEW_DESC triangleMatrixDesc;//Lets keep this artifactname just for fun.
	triangleMatrixDesc.Buffer = triSrvBufferDesc;
	triangleMatrixDesc.Format = DXGI_FORMAT_UNKNOWN;
	triangleMatrixDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	triangleMatrixDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	D3DClass::GetDevice()->CreateShaderResourceView(m_pTriangleMatrices, &triangleMatrixDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * 3));//HOLY SH*T!! THIS IS HARD CODED
	 
	D3D12_BUFFER_SRV lightsSrvBufferDesc = {};
	lightsSrvBufferDesc.FirstElement = 0;
	lightsSrvBufferDesc.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	lightsSrvBufferDesc.NumElements = iNrOfLights;
	lightsSrvBufferDesc.StructureByteStride = sizeof(Light);

	triangleMatrixDesc.Buffer = lightsSrvBufferDesc; 
	D3DClass::GetDevice()->CreateShaderResourceView(m_pLightHeap, &triangleMatrixDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDHlab->GetCPUDescriptorHandleForHeapStart()).Offset(iOffsetSize * 4));


	D3D12_DESCRIPTOR_RANGE	pUAVDescriptorRanges[1];
	pUAVDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;
	pUAVDescriptorRanges[0].NumDescriptors = 3;
	pUAVDescriptorRanges[0].BaseShaderRegister = 0;
	pUAVDescriptorRanges[0].RegisterSpace = 0;
	pUAVDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;


	D3D12_ROOT_DESCRIPTOR_TABLE	UAVDescriptorTable;
	UAVDescriptorTable.NumDescriptorRanges = 1;
	UAVDescriptorTable.pDescriptorRanges = pUAVDescriptorRanges;
	/*
	D3D12_DESCRIPTOR_RANGE lightDescriptorRange[1];
	lightDescriptorRange[0].BaseShaderRegister = 2;
	lightDescriptorRange[0].NumDescriptors = 1;
	lightDescriptorRange[0].OffsetInDescriptorsFromTableStart = 4;
	lightDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	lightDescriptorRange[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE lightsDescriptortable;
	lightsDescriptortable.NumDescriptorRanges = 1;
	lightsDescriptortable.pDescriptorRanges = lightDescriptorRange;*/

	//CREATE SHADER
	Shader* pComputeShader = m_pResourceLoader->CreateComputeShader(L"RayTracerCS.hlsl");
	Shader* pQuadPipeline = m_pResourceLoader->CreateShader(L"QuadVS.hlsl", L"QuadPS.hlsl");
	
	
	D3D12_ROOT_PARAMETER cameraRootParameter;
	cameraRootParameter.Descriptor = CD3DX12_ROOT_DESCRIPTOR(0, 0);
	cameraRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	cameraRootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_PARAMETER rtvRP;
	rtvRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rtvRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rtvRP.DescriptorTable = UAVDescriptorTable;


	D3D12_ROOT_PARAMETER consatntsRP;
	consatntsRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	consatntsRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	consatntsRP.Constants = CD3DX12_ROOT_CONSTANTS(4, 1);

	D3D12_ROOT_DESCRIPTOR triDescriptor;
	triDescriptor.RegisterSpace = 0;
	triDescriptor.ShaderRegister = 1;


	D3D12_ROOT_PARAMETER triangleRP;
	
	triangleRP.Descriptor = triDescriptor;
	triangleRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	triangleRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_DESCRIPTOR lightRootDescriptor;
	lightRootDescriptor.RegisterSpace = 0;
	lightRootDescriptor.ShaderRegister = 2;

	D3D12_ROOT_PARAMETER lightRP;
	lightRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	lightRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	lightRP.Descriptor = lightRootDescriptor;
	//lightRP.DescriptorTable = lightsDescriptortable;


	pComputeShader->AddRootParameter(cameraRootParameter);
	pComputeShader->AddRootParameter(rtvRP);
	pComputeShader->AddRootParameter(consatntsRP);
	pComputeShader->AddRootParameter(triangleRP);
	pComputeShader->AddRootParameter(lightRP);

	D3D12_DESCRIPTOR_RANGE srvRange[1];
	srvRange[0].BaseShaderRegister = 0;
	srvRange[0].NumDescriptors = 3;
	srvRange[0].OffsetInDescriptorsFromTableStart = 0;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE srvTable;
	srvTable.NumDescriptorRanges = 1;
	srvTable.pDescriptorRanges = srvRange;

	D3D12_ROOT_PARAMETER srvRP;
	srvRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	srvRP.DescriptorTable = srvTable;
	srvRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pQuadPipeline->AddSampler(CD3DX12_STATIC_SAMPLER_DESC(0));
	pQuadPipeline->AddRootParameter(srvRP);

	m_pComputeRS = D3DClass::CreateRS(pComputeShader);
	m_pComputePSO = D3DClass::CreateComputePSO(pComputeShader, m_pComputeRS);

	m_pGraphicsRS = D3DClass::CreateRS(pQuadPipeline);
	m_pGraphicsPSO = D3DClass::CreateGraphicsPSO(pQuadPipeline, m_pGraphicsRS);

	delete pComputeShader;
	delete pQuadPipeline;

	return true;
}

void GameClass::Update(Input* input, float dt)
{
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	m_pMainCamera->Update(input, dt, iBackBufferIndex);

}


bool GameClass::Render()
{
	ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pDHlab };
	int iBackBufferIndex = m_pRenderer->GetBackBufferIndex();
	int iDHIncrementSizeRTV = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_pGPUbridge->WaitForPreviousFrame(iBackBufferIndex);
	ID3D12GraphicsCommandList* pCL = m_pGPUbridge->GetFreshCL();

	pCL->SetDescriptorHeaps(1, ppDescriptorHeaps);
	pCL->SetComputeRootSignature(m_pComputeRS);
	pCL->SetPipelineState(m_pComputePSO);
	pCL->SetGraphicsRootSignature(m_pGraphicsRS);

	m_pMainCamera->BindCameraBuffer(0, pCL, iBackBufferIndex);
	pCL->SetComputeRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDHlab->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV * iBackBufferIndex));
	pCL->SetComputeRoot32BitConstant(2, iNrOfTriangles, 0);
	pCL->SetComputeRoot32BitConstant(2, iNrOfLights, 1);
	pCL->SetComputeRoot32BitConstant(2, WindowClass::GetHeight(), 2);
	pCL->SetComputeRoot32BitConstant(2, WindowClass::GetWidth(), 3);
	pCL->SetComputeRootShaderResourceView(3, m_pTriangleMatrices->GetGPUVirtualAddress());//CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDHlab->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV * 3));
	pCL->SetComputeRootShaderResourceView(4, m_pLightHeap->GetGPUVirtualAddress());
	pCL->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDHlab->GetGPUDescriptorHandleForHeapStart()).Offset(iDHIncrementSizeRTV * iBackBufferIndex));
	
	//pCL->SetComputeRoot32BitConstant(2, D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), 1)

	pCL->Dispatch(WindowClass::GetWidth() / 32, WindowClass::GetHeight() / 32, 1);

	m_pRenderer->UnlockNextRTV(pCL);
	pCL->SetPipelineState(m_pGraphicsPSO);
	pCL->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	pCL->DrawInstanced(4, 1, 0, 0);

	m_pRenderer->TransitionCurrentRTVIntoPrecentState(pCL);
	
	m_pGPUbridge->QueueGraphicsCL(pCL);
	m_pGPUbridge->ExecuteGrapichsCLs();

	m_pRenderer->PrecentNextFrame();
	
	return true;
}

void GameClass::CleanUp()
{
	if (m_pMainCamera)
	{
		delete m_pMainCamera;
		m_pMainCamera = nullptr;
	}
	
	if (m_pGPUbridge)
	{
		delete m_pGPUbridge;
		m_pGPUbridge = nullptr;
	}
	
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
	if (m_pResourceLoader)
	{
		delete m_pResourceLoader;
		m_pResourceLoader = nullptr;
	}
}
