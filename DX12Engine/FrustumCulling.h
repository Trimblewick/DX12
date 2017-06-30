#pragma once
#include "Common.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "Camera.h"

class FrustumCulling
{
private:

public:
	FrustumCulling(Camera* camera);
	~FrustumCulling();

	void Update(Camera* camera);
	bool Cull(DirectX::FXMVECTOR pos, float radius);

private:
	DirectX::XMFLOAT4 planes[6];
};
