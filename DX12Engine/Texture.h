#pragma once
#include "Common.h"
#include "d3dx12.h"
#include <d3d12.h>
#include <wincodec.h>
#include "D3dClass.h"
#include "FrameBuffer.h"

class Texture
{
private:
	//for decoding
	DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
	WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
	int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
	int LoadImageDataFromFile();
	
public:
	Texture(LPCWSTR fileName);
	~Texture();

	void GenerateMipMaps(ID3D12Resource* pGPUresource, FrameBuffer* pFrameBuffer);

	BYTE*						GetTextureData();
	D3D12_RESOURCE_DESC*		GetTextureDesc();
	int							GetTextureSize();
	int							GetBytersPerRow();
	int							GetTextureWidth();
	int							GetTextureHeight();


	
private:
	BYTE*						m_pTextureData;
	int							m_iBytesPerRow;
	D3D12_RESOURCE_DESC			m_textureDesc;
	LPCWSTR						m_sFileName;

	int							m_iTextureSize;

	unsigned int				m_uiWidth;
	unsigned int				m_uiHeight;
	
	int							m_iMipLevels;

	CD3DX12_DESCRIPTOR_RANGE	m_pMipmapCbvRange[2];
	CD3DX12_ROOT_PARAMETER		m_pRootParameters[3];
	D3D12_STATIC_SAMPLER_DESC	m_mipmapGenSamplerDesc;

	ID3D12DescriptorHeap*		m_pMipMapDH;
	ID3D12PipelineState*		m_pMipMapPSO;
	ID3D12RootSignature*		m_pMipMapRootSignature;
};

