#include "Object.h"

Object::Object()
{
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.NumParameters = 1;
	desc.NumStaticSamplers = 0;
	desc.pParameters = 0;
	desc.pStaticSamplers = 0;

	ID3DBlob* signatureBlob;
	DxAssert(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, nullptr), S_OK);


}

Object::~Object()
{
}