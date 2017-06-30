#include "FrustumCulling.h"



FrustumCulling::FrustumCulling(Camera* camera)
{

	Update(camera);
}

FrustumCulling::~FrustumCulling()
{

}


void FrustumCulling::Update(Camera* camera)
{
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, camera->GetVPMatrix());

	planes[0].x = mat._14 + mat._11;
	planes[0].y = mat._24 + mat._21;
	planes[0].z = mat._34 + mat._31;
	planes[0].w = mat._44 + mat._41;

	planes[1].x = mat._14 - mat._11;
	planes[1].y = mat._24 - mat._21;
	planes[1].z = mat._34 - mat._31;
	planes[1].w = mat._44 - mat._41;

	planes[2].x = mat._14 + mat._12;
	planes[2].y = mat._24 + mat._22;
	planes[2].z = mat._34 + mat._32;
	planes[2].w = mat._44 + mat._42;

	planes[3].x = mat._14 - mat._12;
	planes[3].y = mat._24 - mat._22;
	planes[3].z = mat._34 - mat._32;
	planes[3].w = mat._44 - mat._42;

	planes[4].x = mat._13;
	planes[4].y = mat._23;
	planes[4].z = mat._33;
	planes[4].w = mat._43;


	planes[5].x = mat._14 + mat._13;
	planes[5].y = mat._24 + mat._23;
	planes[5].z = mat._34 + mat._33;
	planes[5].w = mat._44 + mat._43;

	for (int i = 0; i < 6; ++i)
	{
		DirectX::XMStoreFloat4(&planes[i], DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&planes[i])));
	}

	return;
}

bool FrustumCulling::Cull(DirectX::FXMVECTOR pos, float radius)
{
	for (int i = 0; i < 6; ++i)
	{
		if (DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(&planes[i]), pos)) + planes[i].w + radius <= 0.0f)
			return false;
	}
	return true;
}