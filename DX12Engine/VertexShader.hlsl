
struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal: NORMAL;
};

struct VS_OUTPUT
{
    float4 position: SV_POSITION;
    float4 normal: NORMAL;
};

cbuffer CameraBuffer : register(b0)
{
	float4x4		vpMatrix;
	float3			position;
	float pad1;
	float3			forward;
	float pad2;
	float3			right;
	float pad3;
	float3			up;
	float pad4;
	float4x4		viewMatrix;
	float4x4		projMatrix;
};
/*
cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
	float padding[48];
};*/

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(input.position, vpMatrix);
    output.normal = input.normal;
    return output;
}