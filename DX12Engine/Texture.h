#pragma once
#include "Common.h"
#include "d3dx12.h"
#include <d3d12.h>
#include <wincodec.h>


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
};

