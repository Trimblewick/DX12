#ifndef D3DCLASS_H
#define D3DCLASS_H
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <iostream>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


#include "d3dx12.h"

#include "Common.h"

class D3dClass
{
public:
	D3dClass();
	~D3dClass();

	struct Vertex {
		DirectX::XMFLOAT3 pos;
	};

	ID3D12Device* GetDevice();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsyncstate, bool fullscreen);
	void Shutdown();
	bool Render();

private:
	bool m_bVsyncEnabled;
	ID3D12Device* m_pDevice;
	ID3D12CommandQueue* m_pCommandQ;
	char m_chVideocardDesc[128];
	IDXGISwapChain3* m_pSwapChain;
	ID3D12DescriptorHeap* m_pRtvHeap;
	ID3D12Resource* m_pBackbufferRenderTarget[2];
	unsigned int m_iBufferIndex;
	ID3D12CommandAllocator* m_pCommandAllocator;
	ID3D12GraphicsCommandList* m_pGraphicsCommandList;



	ID3D12PipelineState* m_pPipelineState;
	ID3D12RootSignature* m_pRootSignature;

	

	ID3D12Resource* testVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	ID3D12Fence* m_pFence;
	HANDLE m_fenceEvent;
	unsigned int m_iFrameIndex;
	unsigned long long m_llFenceValue;
	
	
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;


};



#endif // !D3DCLASS_H
