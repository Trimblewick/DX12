#include "Texture.h"


Texture::Texture(LPCWSTR fileName)
{
	m_sFileName = fileName;

	assert(this->LoadImageDataFromFile() > 0);
}

Texture::~Texture()
{
	if (m_pTextureData)
	{
		delete[] m_pTextureData;
		m_pTextureData = nullptr;
	}
	SAFE_RELEASE(m_pMipMapDH);
	SAFE_RELEASE(m_pMipMapPSO);
	SAFE_RELEASE(m_pMipMapRootSignature);
}

BYTE * Texture::GetTextureData()
{
	return m_pTextureData;
}

D3D12_RESOURCE_DESC * Texture::GetTextureDesc()
{
	return &m_textureDesc;
}

int Texture::GetTextureSize()
{
	return m_iTextureSize;
}

int Texture::GetBytersPerRow()
{
	return m_iBytesPerRow;
}

int Texture::GetTextureWidth()
{
	return m_uiWidth;
}

int Texture::GetTextureHeight()
{
	return m_uiHeight;
}


// get the dxgi format equivilent of a wic format
DXGI_FORMAT Texture::GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

	else return DXGI_FORMAT_UNKNOWN;
}

// get a dxgi compatible wic format from another wic format
WICPixelFormatGUID Texture::GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

	else return GUID_WICPixelFormatDontCare;
}

// get the number of bits per pixel for a dxgi format
int Texture::GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

	else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
	else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
}


// load and decode image from file
int Texture::LoadImageDataFromFile()
{
	HRESULT hr;

	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory* pWICFactory;

	// reset decoder, frame and converter since these will be different for each image we load
	IWICBitmapDecoder* pWICDecoder = NULL;
	IWICBitmapFrameDecode* pWICFrame = NULL;
	IWICFormatConverter* pWICConverter = NULL;

	bool imageConverted = false;

	if (pWICFactory == NULL)
	{
		// Initialize the COM library
		CoInitialize(NULL);

		// create the WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pWICFactory)
		);
		if (FAILED(hr)) return 0;
	}

	// load a decoder for the image
	hr = pWICFactory->CreateDecoderFromFilename(
		m_sFileName,                        // Image we want to load in
		NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
		GENERIC_READ,                    // We want to read from this file
		WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
		&pWICDecoder                      // the wic decoder to be created
	);
	if (FAILED(hr)) return 0;

	// get image from decoder (this will decode the "frame")
	hr = pWICDecoder->GetFrame(0, &pWICFrame);
	if (FAILED(hr)) return 0;

	// get wic pixel format of image
	WICPixelFormatGUID pixelFormat;
	hr = pWICFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)) return 0;

	// get size of image
	hr = pWICFrame->GetSize(&m_uiWidth, &m_uiHeight);
	if (FAILED(hr)) return 0;

	//assuming texture is square and of a power of two
	m_iMipLevels = std::log2(m_uiWidth);

	// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
	// out how to implement the support yourself

	// convert wic pixel format to dxgi pixel format
	DXGI_FORMAT dxgiFormat = this->GetDXGIFormatFromWICFormat(pixelFormat);

	// if the format of the image is not a supported dxgi format, try to convert it
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		// get a dxgi compatible wic format from the current image format
		WICPixelFormatGUID convertToPixelFormat = this->GetConvertToWICFormat(pixelFormat);

		// return if no dxgi compatible format was found
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

		// set the dxgi format
		dxgiFormat = this->GetDXGIFormatFromWICFormat(convertToPixelFormat);

		// create the format converter
		hr = pWICFactory->CreateFormatConverter(&pWICConverter);
		if (FAILED(hr)) return 0;

		// make sure we can convert to the dxgi compatible format
		BOOL canConvert = FALSE;
		hr = pWICConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (FAILED(hr) || !canConvert) return 0;

		// do the conversion (wicConverter will contain the converted image)
		hr = pWICConverter->Initialize(pWICFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) return 0;

		// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
		imageConverted = true;
	}

	int bitsPerPixel = this->GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
	m_iBytesPerRow = (m_uiWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = m_iBytesPerRow * m_uiHeight; // total image size in bytes

												 // allocate enough memory for the raw image data, and set imageData to point to that memory
	m_pTextureData = new BYTE[imageSize];//(BYTE*)malloc(imageSize);

	// copy (decoded) raw image data into the newly allocated memory (imageData)
	if (imageConverted)
	{
		// if image format needed to be converted, the wic converter will contain the converted image
		hr = pWICConverter->CopyPixels(0, m_iBytesPerRow, imageSize, m_pTextureData);
		if (FAILED(hr)) return 0;
	}
	else
	{
		// no need to convert, just copy data from the wic frame
		hr = pWICFrame->CopyPixels(0, m_iBytesPerRow, imageSize, m_pTextureData);
		if (FAILED(hr)) return 0;
	}

	// now describe the texture with the information we have obtained from the image
	m_textureDesc = {};
	m_textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_textureDesc.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	m_textureDesc.Width = m_uiWidth; // width of the texture
	m_textureDesc.Height = m_uiHeight; // height of the texture
	m_textureDesc.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	m_textureDesc.MipLevels = 10; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	m_textureDesc.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	m_textureDesc.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	m_textureDesc.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	m_textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	m_textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS; // no flags

	m_iTextureSize = imageSize;				// return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
	return imageSize;
}

void Texture::GenerateMipMaps(ID3D12Resource* pGPUresource, FrameBuffer* pFrameBuffer)
{
	
	m_pMipmapCbvRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	m_pMipmapCbvRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0);

	m_pRootParameters[0].InitAsConstants(2, 0);
	m_pRootParameters[1].InitAsDescriptorTable(1, &m_pMipmapCbvRange[0]);
	m_pRootParameters[2].InitAsDescriptorTable(1, &m_pMipmapCbvRange[1]);

	m_mipmapGenSamplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	m_mipmapGenSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	m_mipmapGenSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	m_mipmapGenSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	m_mipmapGenSamplerDesc.MipLODBias = 0.0f;
	m_mipmapGenSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	m_mipmapGenSamplerDesc.MinLOD = 0.0f;
	m_mipmapGenSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	m_mipmapGenSamplerDesc.MaxAnisotropy = 0;
	m_mipmapGenSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	m_mipmapGenSamplerDesc.ShaderRegister = 0;
	m_mipmapGenSamplerDesc.RegisterSpace = 0;
	m_mipmapGenSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	ID3DBlob* signature;
	ID3DBlob* pEblob;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(m_pRootParameters), m_pRootParameters, 1, &m_mipmapGenSamplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	DxAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &pEblob), S_OK);
	DxAssert(D3DClass::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pMipMapRootSignature)), S_OK);


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 2 * (m_iMipLevels - 1);
	
	DxAssert(D3DClass::GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pMipMapDH)), S_OK);
	UINT uiDescriptorSize = D3DClass::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	ID3DBlob* pCSblob;
	D3D12_SHADER_BYTECODE csByteCode;

	HRESULT hr = D3DCompileFromFile(
		L"MipCS.hlsl",
		nullptr,
		nullptr,
		"main",
		"cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pCSblob,
		&pEblob);
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pEblob->GetBufferPointer());
		return;
	}

	csByteCode.BytecodeLength = pCSblob->GetBufferSize();
	csByteCode.pShaderBytecode = pCSblob->GetBufferPointer();


	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_pMipMapRootSignature;
	psoDesc.CS = csByteCode;
	
	DxAssert(D3DClass::GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_pMipMapPSO)), S_OK);
	SAFE_RELEASE(pCSblob);

	D3D12_SHADER_RESOURCE_VIEW_DESC srcTextureSRVdesc = {};
	srcTextureSRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srcTextureSRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srcTextureSRVdesc.Format = m_textureDesc.Format;
	srcTextureSRVdesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC destTextureUAVdesc = {};
	destTextureUAVdesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	destTextureUAVdesc.Format = m_textureDesc.Format;

	ID3D12GraphicsCommandList* pCL = pFrameBuffer->GetGraphicsCommandList(FrameBuffer::PIPELINES::STANDARD);//D3DClass::GetNewOffGraphicsCommandList(pPSOMipMaps);
	pCL->SetComputeRootSignature(m_pMipMapRootSignature);
	pCL->SetPipelineState(m_pMipMapPSO);
	pCL->SetDescriptorHeaps(1, &m_pMipMapDH);

	pCL->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			pGPUresource, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	CD3DX12_CPU_DESCRIPTOR_HANDLE currentCPUHandle(m_pMipMapDH->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE currentGPUHandle(m_pMipMapDH->GetGPUDescriptorHandleForHeapStart());

	//Union used for shader constants
	struct DWParam
	{
		DWParam(FLOAT f) : Float(f) {}
		DWParam(UINT u) : Uint(u) {}

		void operator= (FLOAT f) { Float = f; }
		void operator= (UINT u) { Uint = u; }

		union
		{
			FLOAT Float;
			UINT Uint;
		};
	};

	for (uint32_t i = 0; i < m_iMipLevels - 1; ++i)
	{
		//assuming textures are squared, make bit shift to get lesser dim.
		unsigned int destWidth = (m_uiWidth >> (i + 1)) > 1 ? m_uiWidth >> (i + 1) : 1;
		unsigned int destHeight = (m_uiHeight >> (i + 1)) > 1 ? m_uiHeight >> (i + 1) : 1;

		
		srcTextureSRVdesc.Texture2D.MostDetailedMip = i;
		D3DClass::GetDevice()->CreateShaderResourceView(pGPUresource, &srcTextureSRVdesc, currentCPUHandle);
		currentCPUHandle.Offset(1, uiDescriptorSize);

		destTextureUAVdesc.Texture2D.MipSlice = i + 1;
		D3DClass::GetDevice()->CreateUnorderedAccessView(pGPUresource, nullptr, &destTextureUAVdesc, currentCPUHandle);
		currentCPUHandle.Offset(1, uiDescriptorSize);

		pCL->SetComputeRoot32BitConstant(0, DWParam(1.0f / destWidth).Uint, 0);
		pCL->SetComputeRoot32BitConstant(0, DWParam(1.0f / destHeight).Uint, 1);

		pCL->SetComputeRootDescriptorTable(1, currentGPUHandle);
		currentGPUHandle.Offset(1, uiDescriptorSize);
		pCL->SetComputeRootDescriptorTable(2, currentGPUHandle);
		currentGPUHandle.Offset(1, uiDescriptorSize);

		unsigned int dx = destWidth / 8 > 1u ? destWidth / 8 : 1u;
		unsigned int dy = destHeight / 8 > 1u ? destHeight / 8 : 1u;


		pCL->Dispatch(dx, dy, 1);

		pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(pGPUresource));
	}
	pCL->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pGPUresource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));




	//pCL->Close();
	//D3DClass::QueueGraphicsCommandList(pCL);
}
