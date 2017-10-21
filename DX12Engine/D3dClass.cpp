
#include "D3dClass.h"



ID3D12Device* D3DClass::s_pDevice;
IDXGIFactory4* D3DClass::s_pDXGIFactory;


D3DClass::D3DClass()
{
}

D3DClass::~D3DClass()
{
	
}

bool D3DClass::Initialize()
{
	HRESULT hr;

	#ifdef BUILD_ENABLE_D3D12_DEBUG
		ID3D12Debug* d;
		D3D12GetDebugInterface(IID_PPV_ARGS(&d));
		d->EnableDebugLayer();
		SAFE_RELEASE(d);
	#endif

	// -- Create the Device -- //

	
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&s_pDXGIFactory));
	if (FAILED(hr))
	{
		return false;
	}

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

	bool adapterFound = false; // set this to true when a good one was found

							   // find first hardware gpu that supports d3d 12
	while (s_pDXGIFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// we dont want a software device
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}

	if (!adapterFound)
	{
		return false;
	}

	// Create the device
	DxAssert(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_pDevice)), S_OK);
	
	return true;
}

void D3DClass::Cleanup()
{
	

	SAFE_RELEASE(s_pDevice);
	SAFE_RELEASE(s_pDXGIFactory);
	
}

ID3D12CommandAllocator * D3DClass::CreateCA(D3D12_COMMAND_LIST_TYPE listType)
{
	ID3D12CommandAllocator* pCA;
	DxAssert(s_pDevice->CreateCommandAllocator(listType, IID_PPV_ARGS(&pCA)), S_OK);
	return pCA;
}

ID3D12Fence * D3DClass::CreateFence(UINT64 ui64InitVal, D3D12_FENCE_FLAGS fenceFlag)
{
	ID3D12Fence* pFence;
	DxAssert(s_pDevice->CreateFence(ui64InitVal, fenceFlag, IID_PPV_ARGS(&pFence)), S_OK);
	return pFence;
}

ID3D12DescriptorHeap * D3DClass::CreateDH(int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	ID3D12DescriptorHeap* pDH;
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	DxAssert(s_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDH)), S_OK);
	return pDH;
}

ID3D12GraphicsCommandList * D3DClass::CreateGaphicsCL(D3D12_COMMAND_LIST_TYPE listType, ID3D12CommandAllocator* pCA)
{
	ID3D12GraphicsCommandList* pCL;
	DxAssert(s_pDevice->CreateCommandList(0, listType, pCA, nullptr, IID_PPV_ARGS(&pCL)), S_OK);
	return pCL;
}

ID3D12CommandQueue* D3DClass::CreateCQ(D3D12_COMMAND_LIST_TYPE listType)
{
	ID3D12CommandQueue* pCQ;

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = listType;

	DxAssert(s_pDevice->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&pCQ)), S_OK);

	return pCQ;
}

IDXGISwapChain3 * D3DClass::CreateSwapChain(DXGI_SWAP_CHAIN_DESC* desc, ID3D12CommandQueue * pCQ)
{
	IDXGISwapChain* pTemp = nullptr;
	IDXGISwapChain3* pSwapChain = nullptr;

	HRESULT hr = s_pDXGIFactory->CreateSwapChain(pCQ, desc, &pTemp);

	pSwapChain = static_cast<IDXGISwapChain3*>(pTemp);

	return pSwapChain;
}

ID3D12Device * D3DClass::GetDevice()
{
	return s_pDevice;
}