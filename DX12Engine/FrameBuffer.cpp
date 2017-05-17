#include "FrameBuffer.h"


FrameBuffer::FrameBuffer()
{
	
}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::Initialize()
{
}

ID3D12GraphicsCommandList * FrameBuffer::m_GetGraphicsCommandList(PIPELINES i)
{
	return m_pGraphicsCommandLists[i];
}
