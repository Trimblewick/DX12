#ifndef CAMERA_H
#define CAMERA_H


#include <d3d12.h>

#include "Common.h"
#include "Input.h"



class Camera
{
public:
	Camera();
	~Camera();



private:
	D3D12_VIEWPORT m_viewPort;
	D3D12_RECT m_scissorRect;
};



#endif