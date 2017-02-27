#ifndef D3DCLASS_H
#define D3DCLASS_H
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <iostream>

#include <d3d12.h>
#include <dxgi1_4.h>


class D3dClass
{
public:
	D3dClass();
	~D3dClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsyncstate, bool fullscreen);
	void Shutdown();
	bool Render();

private:
	bool vsync_enabled;
	ID3D12Device* device;
	ID3D12CommandQueue* commandQ;
	char videocardDesc[128];
	IDXGISwapChain3* swapChain;
	ID3D12DescriptorHeap* rtvHeap;
	ID3D12Resource* backbufferRenderTarget[2];
	unsigned int bufferIndex;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* graphicsCommandList;
	ID3D12PipelineState* pipelineState;
	ID3D12Fence* fence;
	HANDLE fenceEvent;
	unsigned long long fenceValue;
	
	



};



#endif // !D3DCLASS_H
