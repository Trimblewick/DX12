
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

ID3D12Fence * D3DClass::CreateFence()
{
	ID3D12Fence* pFence;
	DxAssert(s_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)), S_OK);
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
	IDXGISwapChain*		pTemp = nullptr;
	IDXGISwapChain3*	pSwapChain = nullptr;

	DxAssert(s_pDXGIFactory->CreateSwapChain(pCQ, desc, &pTemp), S_OK);

	pSwapChain = static_cast<IDXGISwapChain3*>(pTemp);

	return pSwapChain;
}

ID3D12Resource * D3DClass::CreateCommittedResource(D3D12_HEAP_TYPE heapType, UINT iBufferSize, D3D12_RESOURCE_STATES resourceState, LPCWSTR bufferName)
{
	ID3D12Resource* pCommittedResource;

	DxAssert(s_pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(heapType), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), resourceState, nullptr, IID_PPV_ARGS(&pCommittedResource)), S_OK);
	
	if (bufferName != NULL)
		pCommittedResource->SetName(bufferName);

	return pCommittedResource;
}

ID3D12RootSignature * D3DClass::CreateRS(D3D12_ROOT_SIGNATURE_DESC* desc)
{
	ID3DBlob*				pBlob;
	ID3D12RootSignature*	pRootSignature;

	DxAssert(D3D12SerializeRootSignature(desc, D3D_ROOT_SIGNATURE_VERSION_1, &pBlob, nullptr), S_OK);
	DxAssert(s_pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature)), S_OK);

	return pRootSignature;
}

ID3D12PipelineState * D3DClass::CreateGraphicsPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc)
{
	ID3D12PipelineState*	pPSO;

	DxAssert(s_pDevice->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(&pPSO)), S_OK);

	return pPSO;
}

ID3D12Device * D3DClass::GetDevice()
{
	return s_pDevice;
}